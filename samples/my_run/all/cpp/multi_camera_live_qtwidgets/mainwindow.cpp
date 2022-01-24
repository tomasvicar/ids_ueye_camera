/*!
 * \file    mainwindow.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-05-18
 * \since   1.1.6
 *
 * \brief   This application demonstrates how to use the IDS peak API
 *          combined with a Qt widgets GUI to display images from multiple
 *          GenICam compatible devices.
 *
 * \version 1.2.2
 *
 * Copyright (C) 2020 - 2021, IDS Imaging Development Systems GmbH.
 *
 * The information in this document is subject to change without notice
 * and should not be construed as a commitment by IDS Imaging Development Systems GmbH.
 * IDS Imaging Development Systems GmbH does not assume any responsibility for any errors
 * that may appear in this document.
 *
 * This document, or source code, is provided solely as an example of how to utilize
 * IDS Imaging Development Systems GmbH software libraries in a sample application.
 * IDS Imaging Development Systems GmbH does not assume any responsibility
 * for the use or reliability of any portion of this document.
 *
 * General permission to copy or modify is hereby granted.
 */

#include "mainwindow.h"

#include "acquisitionworker.h"
#include "displaywindow.h"

#include <peak_ipl/peak_ipl.hpp>

#include <peak/peak.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QMessageBox>
#include <QThread>
#include <QWidget>

#include <cstdint>

#define VERSION "1.2.2"

#define MAX_NUMBER_OF_DEVICES 3

#define MAXIMUM_THROUGHPUT_LIMIT 125000000


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    m_centralWidget = new QWidget(this);
    m_layout = new QVBoxLayout();
    m_centralWidget->setLayout(m_layout);
    setCentralWidget(m_centralWidget);

    m_vecDevices.clear();

    m_statusBarLabelVersion = nullptr;
    m_statusBarLabelAboutQt = nullptr;
    m_statusBarLayout = nullptr;
    m_statusBar = nullptr;

    // Initialize peak library
    peak::Library::Initialize();

    if (OpenDevices())
    {
        try
        {
            int posX = 100;
            int posY = 100;

            for (auto deviceElem : m_vecDevices)
            {
                deviceElem->pixelFormat = peak::ipl::PixelFormatName::BGRa8;
                // deviceElem->pixelFormat = peak::ipl::PixelFormatName::RGB8;
                // deviceElem->pixelFormat = peak::ipl::PixelFormatName::RGB10p32;
                // deviceElem->pixelFormat = peak::ipl::PixelFormatName::BayerRG8;
                // deviceElem->pixelFormat = peak::ipl::PixelFormatName::Mono8;

                // Create display window that will show the image and the counters
                deviceElem->displayWindow = new DisplayWindow(nullptr, posX += 100, posY += 100, 500,
                    deviceElem->imageWidth, deviceElem->imageHeight, deviceElem->pixelFormat);

                // Create worker thread that waits for new images from the camera
                deviceElem->acquisitionWorker = new AcquisitionWorker(this, deviceElem->displayWindow,
                    deviceElem->dataStream, deviceElem->pixelFormat, static_cast<size_t>(deviceElem->imageWidth),
                    static_cast<size_t>(deviceElem->imageHeight));

                deviceElem->acquisitionWorker->moveToThread(&deviceElem->acquisitionThread);

                // Connect the signal from the worker thread to update the corresponding display the Display class
                connect(deviceElem->acquisitionWorker, &AcquisitionWorker::ImageReceived, deviceElem->displayWindow,
                    &DisplayWindow::UpdateDisplay);

                // Connect the signal from the worker thread when the counters have changed with the update slot in the
                // MainWindow class
                connect(deviceElem->acquisitionWorker, &AcquisitionWorker::UpdateCounters, deviceElem->displayWindow,
                    &DisplayWindow::UpdateCounters);

                // Call start function of m_acquisitionWorker when thread starts
                connect(&deviceElem->acquisitionThread, &QThread::started, deviceElem->acquisitionWorker,
                    &AcquisitionWorker::Start);

                // Start thread execution
                deviceElem->acquisitionThread.start();

                // Lock critical features to prevent them from changing during acquisition
                deviceElem->nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("TLParamsLocked")
                    ->SetValue(1);

                // Start acquisition of the opened device
                deviceElem->dataStream->StartAcquisition();
                deviceElem->nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")
                    ->Execute();
            }
        }
        catch (const std::exception& e)
        {
            QMessageBox::information(this, "Exception", e.what(), QMessageBox::Ok);
        }
    }
    else
    {
        DestroyAll();
        peak::Library::Close();
        exit(0);
    }

    CreateStatusBar();

    // Set minimum window size
    this->setMinimumSize(300, 100);
}


MainWindow::~MainWindow()
{
    DestroyAll();

    // Close peak library
    peak::Library::Close();
}


void MainWindow::closeEvent(QCloseEvent*)
{
    DestroyAll();
}


void MainWindow::DestroyAll()
{
    for (auto deviceElem : m_vecDevices)
    {
        if (deviceElem->acquisitionWorker)
        {
            deviceElem->acquisitionWorker->Stop();
            deviceElem->acquisitionThread.quit();
            deviceElem->acquisitionThread.wait();

            delete deviceElem->acquisitionWorker;
            deviceElem->acquisitionWorker = nullptr;
        }

        if (deviceElem->displayWindow)
        {
            delete deviceElem->displayWindow;
            deviceElem->displayWindow = nullptr;
        }
    }

    if (m_statusBarLabelVersion)
    {
        delete m_statusBarLabelVersion;
        m_statusBarLabelVersion = nullptr;
    }

    if (m_statusBarLabelAboutQt)
    {
        delete m_statusBarLabelAboutQt;
        m_statusBarLabelAboutQt = nullptr;
    }

    if (m_statusBarLayout)
    {
        delete m_statusBarLayout;
        m_statusBarLayout = nullptr;
    }

    if (m_statusBar)
    {
        delete m_statusBar;
        m_statusBar = nullptr;
    }

    if (m_layout)
    {
        delete m_layout;
        m_layout = nullptr;
    }

    if (m_centralWidget)
    {
        delete m_centralWidget;
        m_centralWidget = nullptr;
    }

    CloseDevices();
}


bool MainWindow::OpenDevices()
{
    try
    {
        // Create instance of the device manager
        auto& deviceManager = peak::DeviceManager::Instance();

        // Update the device manager
        deviceManager.Update();

        // Return if no device was found
        if (deviceManager.Devices().empty())
        {
            QMessageBox::critical(this, "Error", "No device found", QMessageBox::Ok);
            return false;
        }

        // Open the first N openable devices in the device manager's device list
        int opened = 0;

        // The number of GEV cameras is needed later
        int numberOfGevCameras = 0;

        for (size_t i = 0; i < deviceManager.Devices().size(); ++i)
        {
            if (deviceManager.Devices().at(i)->IsOpenable())
            {
                auto device = deviceManager.Devices().at(i)->OpenDevice(peak::core::DeviceAccessType::Control);
                if (device)
                {
                    // Create vector element
                    std::shared_ptr<DeviceContext> deviceElem = std::make_shared<DeviceContext>();
                    deviceElem->device = device;

                    // Open standard data stream
                    auto dataStreams = device->DataStreams();
                    if (dataStreams.empty())
                    {
                        QMessageBox::critical(this, "Error", "Device has no DataStream", QMessageBox::Ok);
                        device.reset();
                        return false;
                    }

                    deviceElem->dataStream = dataStreams.at(0)->OpenDataStream();

                    // Get nodemap of remote device for all accesses to the genicam nodemap tree
                    deviceElem->nodemapRemoteDevice = device->RemoteDevice()->NodeMaps().at(0);

                    // To prepare for untriggered continuous image acquisition, load the default user set if available
                    // and wait until execution is finished
                    try
                    {
                        deviceElem->nodemapRemoteDevice
                            ->FindNode<peak::core::nodes::EnumerationNode>("UserSetSelector")
                            ->SetCurrentEntry("Default");
                        deviceElem->nodemapRemoteDevice
                            ->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")
                            ->Execute();
                        deviceElem->nodemapRemoteDevice
                            ->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")
                            ->WaitUntilDone();
                    }
                    catch (peak::core::Exception const&)
                    {
                        // Cannot load userset default. Ingnore and try to continue.
                    }

                    // Get the payload size for correct buffer allocation
                    auto payloadSize = deviceElem->nodemapRemoteDevice
                                           ->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")
                                           ->Value();

                    // Get the minimum number of buffers that must be announced
                    auto bufferCountMin = deviceElem->dataStream->NumBuffersAnnouncedMinRequired();

                    // Allocate and announce image buffers and queue them
                    for (size_t bufferCount = 0; bufferCount < bufferCountMin; ++bufferCount)
                    {
                        auto buffer = deviceElem->dataStream->AllocAndAnnounceBuffer(
                            static_cast<size_t>(payloadSize), nullptr);
                        deviceElem->dataStream->QueueBuffer(buffer);
                    }

                    // Get the sensor size
                    deviceElem->imageWidth = static_cast<int>(
                        deviceElem->nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("Width")
                            ->Value());
                    deviceElem->imageHeight = static_cast<int>(
                        deviceElem->nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("Height")
                            ->Value());

                    if ("GEV" == deviceElem->device->ParentInterface()->TLType())
                    {
                        numberOfGevCameras++;
                    }

                    // Push vector element into the device vector
                    m_vecDevices.push_back(deviceElem);

                    opened++;
                }

                if (MAX_NUMBER_OF_DEVICES == opened)
                {
                    break;
                }
            }
        }

        if (m_vecDevices.empty())
        {
            QMessageBox::critical(this, "Error", "No device openable - maybe already in use?", QMessageBox::Ok);
            return false;
        }

        /****************************************/
        /* GEV bandwidth management             */
        /****************************************/

        if (0 != numberOfGevCameras)
        {
            // Divide the maximum bandwidth by number of GEV cameras
            int64_t deviceLinkThroughputLimit = static_cast<int64_t>(MAXIMUM_THROUGHPUT_LIMIT / numberOfGevCameras);

            for (const auto& deviceElem : m_vecDevices)
            {
                if ("GEV" == deviceElem->device->ParentInterface()->TLType())
                {
                    // Try to adjust the DeviceLinkThoughputLimit
                    try
                    {
                        // Get range of the device link throughput limit
                        int64_t deviceLinkThroughputLimitRange_Min =
                            deviceElem->nodemapRemoteDevice
                                ->FindNode<peak::core::nodes::IntegerNode>("DeviceLinkThroughputLimit")
                                ->Minimum();
                        int64_t deviceLinkThroughputLimitRange_Max =
                            deviceElem->nodemapRemoteDevice
                                ->FindNode<peak::core::nodes::IntegerNode>("DeviceLinkThroughputLimit")
                                ->Maximum();

                        // Check the value
                        if (deviceLinkThroughputLimit < deviceLinkThroughputLimitRange_Min)
                        {
                            deviceLinkThroughputLimit = deviceLinkThroughputLimitRange_Min;
                        }
                        else if (deviceLinkThroughputLimit > deviceLinkThroughputLimitRange_Max)
                        {
                            deviceLinkThroughputLimit = deviceLinkThroughputLimitRange_Max;
                        }

                        // Set new throughput limit
                        deviceElem->nodemapRemoteDevice
                            ->FindNode<peak::core::nodes::IntegerNode>("DeviceLinkThroughputLimit")
                            ->SetValue(deviceLinkThroughputLimit);

                        // The maximum possible framerate changes depending on the throughput limit
                        double frameRateLimit = deviceElem->nodemapRemoteDevice
                                                    ->FindNode<peak::core::nodes::FloatNode>(
                                                        "DeviceLinkAcquisitionFrameRateLimit")
                                                    ->Value()
                            - 0.5;

                        // Get the framerate range
                        double framerateMin = deviceElem->nodemapRemoteDevice
                                                  ->FindNode<peak::core::nodes::FloatNode>(
                                                      "AcquisitionFrameRate")
                                                  ->Minimum();
                        double framerateMax = deviceElem->nodemapRemoteDevice
                                                  ->FindNode<peak::core::nodes::FloatNode>(
                                                      "AcquisitionFrameRate")
                                                  ->Maximum();

                        // Check framerate limit
                        if (frameRateLimit < framerateMin)
                        {
                            frameRateLimit = framerateMin;
                        }
                        else if (frameRateLimit > framerateMax)
                        {
                            frameRateLimit = framerateMax;
                        }

                        // Set framerate to limit minus safety buffer of 0.5 frames
                        deviceElem->nodemapRemoteDevice
                            ->FindNode<peak::core::nodes::FloatNode>("AcquisitionFrameRate")
                            ->SetValue(frameRateLimit);
                    }
                    catch (peak::core::Exception const&)
                    {
                        QMessageBox::information(this, "Warning",
                            "Unable to optimize bandwidth. Program will continue, but performance might not be "
                            "optimal.",
                            QMessageBox::Ok);
                    }
                }
            }
        }

        return true;
    }
    catch (const std::exception& e)
    {
        QMessageBox::information(this, "Exception", e.what(), QMessageBox::Ok);
    }

    return false;
}


void MainWindow::CloseDevices()
{
    for (auto deviceElem : m_vecDevices)
    {
        try
        {
            if (deviceElem->dataStream->IsGrabbing())
            {
                deviceElem->nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStop")
                    ->Execute();
                deviceElem->nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStop")
                    ->WaitUntilDone();
            }
        }
        catch (const std::exception& e)
        {
            QMessageBox::information(this, "Exception", e.what(), QMessageBox::Ok);
        }

        // if data stream was opened, try to stop it and revoke its image buffers
        if (deviceElem->dataStream)
        {
            try
            {
                if (deviceElem->dataStream->IsGrabbing())
                {
                    deviceElem->dataStream->KillWait();
                    deviceElem->dataStream->StopAcquisition(peak::core::AcquisitionStopMode::Default);
                }

                deviceElem->dataStream->Flush(peak::core::DataStreamFlushMode::DiscardAll);

                for (const auto& buffer : deviceElem->dataStream->AnnouncedBuffers())
                {
                    deviceElem->dataStream->RevokeBuffer(buffer);
                }

                // Unlock parameters after acquisition stop
                deviceElem->nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("TLParamsLocked")
                    ->SetValue(0);

                deviceElem->dataStream = nullptr;
                deviceElem->nodemapRemoteDevice = nullptr;
                deviceElem->device = nullptr;
            }
            catch (const std::exception& e)
            {
                QMessageBox::information(this, "Exception", e.what(), QMessageBox::Ok);
            }
        }
    }

    m_vecDevices.clear();
}


void MainWindow::CreateStatusBar()
{
    m_statusBar = new QWidget(this);
    m_statusBarLayout = new QHBoxLayout;
    m_statusBarLayout->setContentsMargins(0, 0, 0, 0);

    m_statusBarLabelVersion = new QLabel(m_statusBar);
    m_statusBarLabelVersion->setText(("multi_camera_live_qtwidgets v" VERSION));
    m_statusBarLabelVersion->setAlignment(Qt::AlignLeft);
    m_statusBarLayout->addWidget(m_statusBarLabelVersion);

    m_statusBarLabelAboutQt = new QLabel(m_statusBar);
    m_statusBarLabelAboutQt->setObjectName("aboutQt");
    m_statusBarLabelAboutQt->setText(R"(<a href="#aboutQt">About Qt</a>)");
    m_statusBarLabelAboutQt->setAlignment(Qt::AlignRight);
    connect(m_statusBarLabelAboutQt, SIGNAL(linkActivated(QString)), this, SLOT(OnAboutQt(QString)));
    m_statusBarLayout->addWidget(m_statusBarLabelAboutQt);
    m_statusBar->setLayout(m_statusBarLayout);

    m_layout->addWidget(m_statusBar);
}


void MainWindow::OnAboutQt(const QString& link)
{
    if (link == "#aboutQt")
    {
        QMessageBox::aboutQt(this, "About Qt");
    }
}
