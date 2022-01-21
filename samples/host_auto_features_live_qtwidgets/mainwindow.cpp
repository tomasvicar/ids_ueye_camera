/*!
 * \file    mainwindow.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2021-03-12
 * \since   1.2.0
 *
 * \version 1.0.1
 *
 * Copyright (C) 2021, IDS Imaging Development Systems GmbH.
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
#include "display.h"

#include <peak_ipl/peak_ipl.hpp>

#include <peak/peak.hpp>

#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>
#include <cstdint>

#define VERSION "1.0.1"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_gainAutoSupported{ false }
    , m_balanceWhiteAutoSupported{ false }
    , m_centralWidget{ nullptr }
    , m_labelInfo{ nullptr }
    , m_labelVersion{ nullptr }
    , m_labelAboutQt{ nullptr }
    , m_layoutStatusBar{ nullptr }
    , m_statusBar{ nullptr }
    , m_display{ nullptr }
    , m_labelExposureAuto{ nullptr }
    , m_labelExposureAutoOff{ nullptr }
    , m_radioExposureAutoOff{ nullptr }
    , m_labelExposureAutoOnce{ nullptr }
    , m_radioExposureAutoOnce{ nullptr }
    , m_labelExposureAutoContinuous{ nullptr }
    , m_radioExposureAutoContinuous{ nullptr }
    , m_groupExposureAuto{ nullptr }
    , m_labelGainAuto{ nullptr }
    , m_labelGainAutoOff{ nullptr }
    , m_radioGainAutoOff{ nullptr }
    , m_labelGainAutoOnce{ nullptr }
    , m_radioGainAutoOnce{ nullptr }
    , m_labelGainAutoContinuous{ nullptr }
    , m_radioGainAutoContinuous{ nullptr }
    , m_groupGainAuto{ nullptr }
    , m_labelBalanceWhiteAuto{ nullptr }
    , m_labelBalanceWhiteAutoOff{ nullptr }
    , m_radioBalanceWhiteAutoOff{ nullptr }
    , m_labelBalanceWhiteAutoOnce{ nullptr }
    , m_radioBalanceWhiteAutoOnce{ nullptr }
    , m_labelBalanceWhiteAutoContinuous{ nullptr }
    , m_radioBalanceWhiteAutoContinuous{ nullptr }
    , m_groupBalanceWhiteAuto{ nullptr }
    , m_buttonReset{ nullptr }
    , m_labelSkipFrames{ nullptr }
    , m_spinBoxSkipFrames{ nullptr }
    , m_layoutRadioButtonsExposureAuto{ nullptr }
    , m_layoutRadioButtonsGainAuto{ nullptr }
    , m_layoutRadioButtonsBalanceWhiteAuto{ nullptr }
    , m_layoutSkipFramesControls{ nullptr }
    , m_layoutControls{ nullptr }
    , m_layout{ nullptr }
    , m_acquisitionWorker{ nullptr }
{
    m_centralWidget = new QWidget(this);
    m_layout = new QVBoxLayout;
    m_centralWidget->setLayout(m_layout);
    setCentralWidget(m_centralWidget);

    m_frameRateSet = 0;

    // initialize IDS peak API library
    peak::Library::Initialize();

    if (OpenDevice())
    {
        try
        {
            // Create a display for the camera image
            m_display = new Display(m_centralWidget);
            m_layout->addWidget(m_display);

            // Create worker thread that waits for new images from the camera
            m_acquisitionWorker = new AcquisitionWorker();
            m_acquisitionWorker->SetDataStream(m_dataStream);
            m_acquisitionWorker->moveToThread(&m_acquisitionThread);

            m_autoFeaturesManager.SetNodemapRemoteDevice(m_nodemapRemoteDevice);
            m_autoFeaturesManager.SetExposureAutoMode(AutoFeaturesManager::ExposureAutoMode::Continuous);

            // The camera supports master gain -> GainAuto is supported
            m_gainAutoSupported = AutoFeaturesManager::GainType::None != m_autoFeaturesManager.GetGainTypeAll();

            // The camera supports color gains -> BalanceWhiteAuto is supported
            m_balanceWhiteAutoSupported = AutoFeaturesManager::GainType::None
                != m_autoFeaturesManager.GetGainTypeColor();

            if (m_gainAutoSupported)
            {
                m_autoFeaturesManager.SetGainAutoMode(AutoFeaturesManager::GainAutoMode::Continuous);
            }

            if (m_balanceWhiteAutoSupported)
            {
                m_autoFeaturesManager.SetBalanceWhiteAutoMode(AutoFeaturesManager::BalanceWhiteAutoMode::Continuous);
            }

            m_acquisitionWorker->SetAutoFeaturesManager(&m_autoFeaturesManager);

            // worker must be started, when the acquisition starts, and deleted, when the worker thread finishes
            connect(&m_acquisitionThread, &QThread::started, m_acquisitionWorker, &AcquisitionWorker::Start);

            // Connect the signal from the worker thread when a new image was received with the display update slot in
            // the Display class
            connect(m_acquisitionWorker, &AcquisitionWorker::imageReceived, m_display, &Display::onImageReceived);

            // Connect the signal from the worker thread when the counters have changed with the update slot in the
            // MainWindow class
            connect(m_acquisitionWorker, &AcquisitionWorker::counterUpdated, this, &MainWindow::OnCounterUpdated);

            // Start thread execution
            m_acquisitionThread.start();
        }
        catch (const std::exception& e)
        {
            QMessageBox::information(this, "Exception", e.what(), QMessageBox::Ok);
        }
    }
    else
    {
        DestroyAll();
        exit(0);
    }

    CreateControls();

    UpdateAutoControls();

    // Set minimum window size
    this->setMinimumSize(700, 500);

    connect(m_acquisitionWorker, SIGNAL(updateAutoControls()), this, SLOT(UpdateAutoControls()), Qt::UniqueConnection);
}


MainWindow::~MainWindow()
{
    DestroyAll();
}


void MainWindow::DeleteObject(QObject* object)
{
    if (object)
    {
        delete object;
    }
}


void MainWindow::DeleteLayout(QLayout* layout)
{
    if (layout)
    {
        while (layout->count() > 0)
        {
            QLayoutItem* item = layout->itemAt(0);
            if (item)
            {
                if (item->widget())
                {
                    delete item->widget();
                }

                layout->removeItem(item);
            }
        }

        delete layout;
    }
}


void MainWindow::DestroyAll()
{
    if (m_acquisitionWorker)
    {
        m_acquisitionWorker->Stop();
        m_acquisitionThread.quit();
        m_acquisitionThread.wait();
    }

    DeleteObject(m_acquisitionWorker);

    CloseDevice();

    // close IDS peak API library
    peak::Library::Close();

    DeleteObject(m_display);
    DeleteLayout(m_layoutStatusBar);
    DeleteObject(m_statusBar);
    DeleteLayout(m_layoutRadioButtonsExposureAuto);
    DeleteLayout(m_layoutRadioButtonsGainAuto);
    DeleteLayout(m_layoutRadioButtonsBalanceWhiteAuto);
    DeleteLayout(m_layoutSkipFramesControls);
    DeleteLayout(m_layoutControls);
    DeleteObject(m_groupExposureAuto);
    DeleteObject(m_groupGainAuto);
    DeleteObject(m_groupBalanceWhiteAuto);
    DeleteObject(m_layout);
    DeleteObject(m_centralWidget);
}


bool MainWindow::OpenDevice()
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

        // open the first openable device in the device manager's device list
        size_t deviceCount = deviceManager.Devices().size();
        for (size_t i = 0; i < deviceCount; ++i)
        {
            if (deviceManager.Devices().at(i)->IsOpenable())
            {
                m_device = deviceManager.Devices().at(i)->OpenDevice(peak::core::DeviceAccessType::Control);

                // stop after the first opened device
                break;
            }
            else if (i == (deviceCount - 1))
            {
                QMessageBox::critical(this, "Error", "Device could not be opened", QMessageBox::Ok);
                return false;
            }
        }

        if (m_device)
        {
            // Open standard data stream
            auto dataStreams = m_device->DataStreams();
            if (dataStreams.empty())
            {
                QMessageBox::critical(this, "Error", "Device has no DataStream", QMessageBox::Ok);
                m_device.reset();
                return false;
            }

            m_dataStream = dataStreams.at(0)->OpenDataStream();

            // Get nodemap of remote device for all accesses to the genicam nodemap tree
            m_nodemapRemoteDevice = m_device->RemoteDevice()->NodeMaps().at(0);

            // To prepare for untriggered continuous image acquisition, load the default user set if available
            // and wait until execution is finished
            try
            {
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("UserSetSelector")
                    ->SetCurrentEntry("Default");

                m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->Execute();
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")
                    ->WaitUntilDone();
            }
            catch (const peak::core::NotFoundException&)
            {
                // UserSet is not available
            }

            bool color = false;

            // Set a supported source bayer 8 bit pixelformat which is suitable for the auto feature algorithms
            for (const auto& pixelFormat :
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("PixelFormat")->Entries())
            {
                std::string pixelFormatName = pixelFormat->SymbolicValue();

                if ((("BayerRG8" == pixelFormatName) || ("BayerGR8" == pixelFormatName)
                        || ("BayerGB8" == pixelFormatName) || ("BayerBG8" == pixelFormatName))
                    && (peak::core::nodes::NodeAccessStatus::NotAvailable != pixelFormat->AccessStatus()
                        && peak::core::nodes::NodeAccessStatus::NotImplemented != pixelFormat->AccessStatus()))
                {
                    m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("PixelFormat")
                        ->SetCurrentEntry(pixelFormatName);
                    color = true;
                    break;
                }
            }

            // Set the source format to mono8 for a mono camera
            if (!color)
            {
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("PixelFormat")
                    ->SetCurrentEntry("Mono8");
            }

            // Get the payload size for correct buffer allocation
            auto payloadSize = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")
                                   ->Value();

            // Get the minimum number of buffers that must be announced
            auto bufferCountMax = m_dataStream->NumBuffersAnnouncedMinRequired();

            // Allocate and announce image buffers and queue them
            for (size_t bufferCount = 0; bufferCount < bufferCountMax; ++bufferCount)
            {
                auto buffer = m_dataStream->AllocAndAnnounceBuffer(static_cast<size_t>(payloadSize), nullptr);
                m_dataStream->QueueBuffer(buffer);
            }

            // Set exposure to minimum
            double exposure = m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("ExposureTime")
                                  ->Minimum();
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("ExposureTime")->SetValue(exposure);

            // Set frame rate
            const auto frameRate = (std::min)(
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("AcquisitionFrameRate")
                    ->Maximum(),
                25.0);
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("AcquisitionFrameRate")
                ->SetValue(frameRate);

            m_frameRateSet =
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("AcquisitionFrameRate")->Value();

            return true;
        }
    }
    catch (const std::exception& e)
    {
        QMessageBox::information(this, "Exception", e.what(), QMessageBox::Ok);
    }

    return false;
}


void MainWindow::CloseDevice()
{
    // if device was opened, try to stop acquisition
    if (m_device)
    {
        try
        {
            auto remoteNodeMap = m_device->RemoteDevice()->NodeMaps().at(0);
            remoteNodeMap->FindNode<peak::core::nodes::CommandNode>("AcquisitionStop")->Execute();
        }
        catch (const std::exception& e)
        {
            QMessageBox::information(this, "Exception", e.what(), QMessageBox::Ok);
        }
    }

    // if data stream was opened, try to stop it and revoke its image buffers
    if (m_dataStream)
    {
        try
        {
            m_dataStream->KillWait();
            m_dataStream->StopAcquisition(peak::core::AcquisitionStopMode::Default);
            m_dataStream->Flush(peak::core::DataStreamFlushMode::DiscardAll);

            for (const auto& buffer : m_dataStream->AnnouncedBuffers())
            {
                m_dataStream->RevokeBuffer(buffer);
            }
        }
        catch (const std::exception& e)
        {
            QMessageBox::information(this, "Exception", e.what(), QMessageBox::Ok);
        }
    }
}


void MainWindow::CreateControls()
{
    m_labelInfo = new QLabel();
    m_labelInfo->setAlignment(Qt::AlignLeft);

    m_labelVersion = new QLabel();
    m_labelVersion->setText(("host_auto_features_live_qtwidgets v" VERSION));
    m_labelVersion->setAlignment(Qt::AlignRight);

    m_labelAboutQt = new QLabel();
    m_labelAboutQt->setObjectName("aboutQt");
    m_labelAboutQt->setText(R"(<a href="#aboutQt">About Qt</a>)");
    m_labelAboutQt->setAlignment(Qt::AlignRight);
    connect(m_labelAboutQt, SIGNAL(linkActivated(QString)), this, SLOT(OnAboutQtLinkActivated(QString)));

    m_layoutStatusBar = new QHBoxLayout;
    m_layoutStatusBar->setContentsMargins(0, 0, 0, 0);
    m_layoutStatusBar->addWidget(m_labelInfo);
    m_layoutStatusBar->addStretch();
    m_layoutStatusBar->addWidget(m_labelVersion);
    m_layoutStatusBar->addWidget(m_labelAboutQt);

    m_statusBar = new QWidget(centralWidget());
    m_statusBar->setLayout(m_layoutStatusBar);

    // Controls for ExposureAuto
    m_labelExposureAuto = new QLabel(this);
    m_labelExposureAuto->setText("Exposure Auto");
    m_labelExposureAuto->setFixedHeight(24);

    m_labelExposureAutoOff = new QLabel("Off", this);
    m_labelExposureAutoOff->setFixedWidth(16);
    m_radioExposureAutoOff = new QRadioButton(this);
    m_radioExposureAutoOff->setFixedWidth(14);

    m_labelExposureAutoOnce = new QLabel("Once", this);
    m_labelExposureAutoOnce->setFixedWidth(26);
    m_radioExposureAutoOnce = new QRadioButton(this);
    m_radioExposureAutoOnce->setFixedWidth(14);

    m_labelExposureAutoContinuous = new QLabel("Continuous", this);
    m_labelExposureAutoContinuous->setFixedWidth(56);
    m_radioExposureAutoContinuous = new QRadioButton(this);
    m_radioExposureAutoContinuous->setFixedWidth(14);

    m_groupExposureAuto = new QButtonGroup(this);
    m_groupExposureAuto->addButton(
        m_radioExposureAutoOff, static_cast<int>(AutoFeaturesManager::ExposureAutoMode::Off));
    m_groupExposureAuto->addButton(
        m_radioExposureAutoOnce, static_cast<int>(AutoFeaturesManager::ExposureAutoMode::Once));
    m_groupExposureAuto->addButton(
        m_radioExposureAutoContinuous, static_cast<int>(AutoFeaturesManager::ExposureAutoMode::Continuous));

    connect(
        m_groupExposureAuto, SIGNAL(buttonClicked(int)), this, SLOT(OnRadioExposureAuto(int)), Qt::UniqueConnection);

    if (m_gainAutoSupported)
    {
        // Controls for GainAuto
        m_labelGainAuto = new QLabel(this);
        m_labelGainAuto->setText("Gain Auto");
        m_labelGainAuto->setFixedHeight(24);

        m_labelGainAutoOff = new QLabel("Off", this);
        m_labelGainAutoOff->setFixedWidth(16);
        m_radioGainAutoOff = new QRadioButton(this);
        m_radioGainAutoOff->setFixedWidth(14);

        m_labelGainAutoOnce = new QLabel("Once", this);
        m_labelGainAutoOnce->setFixedWidth(26);
        m_radioGainAutoOnce = new QRadioButton(this);
        m_radioGainAutoOnce->setFixedWidth(14);

        m_labelGainAutoContinuous = new QLabel("Continuous", this);
        m_labelGainAutoContinuous->setFixedWidth(56);
        m_radioGainAutoContinuous = new QRadioButton(this);
        m_radioGainAutoContinuous->setFixedWidth(14);

        m_groupGainAuto = new QButtonGroup(this);
        m_groupGainAuto->addButton(m_radioGainAutoOff, static_cast<int>(AutoFeaturesManager::GainAutoMode::Off));
        m_groupGainAuto->addButton(m_radioGainAutoOnce, static_cast<int>(AutoFeaturesManager::GainAutoMode::Once));
        m_groupGainAuto->addButton(
            m_radioGainAutoContinuous, static_cast<int>(AutoFeaturesManager::GainAutoMode::Continuous));

        connect(m_groupGainAuto, SIGNAL(buttonClicked(int)), this, SLOT(OnRadioGainAuto(int)), Qt::UniqueConnection);
    }

    if (m_balanceWhiteAutoSupported)
    {
        // Controls for BalanceWhiteAuto
        m_labelBalanceWhiteAuto = new QLabel(this);
        m_labelBalanceWhiteAuto->setText("Balance White Auto");
        m_labelBalanceWhiteAuto->setFixedHeight(24);

        m_labelBalanceWhiteAutoOff = new QLabel("Off", this);
        m_labelBalanceWhiteAutoOff->setFixedWidth(16);
        m_radioBalanceWhiteAutoOff = new QRadioButton(this);
        m_radioBalanceWhiteAutoOff->setFixedHeight(14);

        m_labelBalanceWhiteAutoOnce = new QLabel("Once", this);
        m_labelBalanceWhiteAutoOnce->setFixedWidth(26);
        m_radioBalanceWhiteAutoOnce = new QRadioButton(this);
        m_radioBalanceWhiteAutoOnce->setFixedWidth(14);

        m_labelBalanceWhiteAutoContinuous = new QLabel("Continuous", this);
        m_labelBalanceWhiteAutoContinuous->setFixedWidth(56);
        m_radioBalanceWhiteAutoContinuous = new QRadioButton(this);
        m_radioBalanceWhiteAutoContinuous->setFixedWidth(14);

        m_groupBalanceWhiteAuto = new QButtonGroup(this);
        m_groupBalanceWhiteAuto->addButton(
            m_radioBalanceWhiteAutoOff, static_cast<int>(AutoFeaturesManager::BalanceWhiteAutoMode::Off));
        m_groupBalanceWhiteAuto->addButton(
            m_radioBalanceWhiteAutoOnce, static_cast<int>(AutoFeaturesManager::BalanceWhiteAutoMode::Once));
        m_groupBalanceWhiteAuto->addButton(
            m_radioBalanceWhiteAutoContinuous, static_cast<int>(AutoFeaturesManager::BalanceWhiteAutoMode::Continuous));

        connect(m_groupBalanceWhiteAuto, SIGNAL(buttonClicked(int)), this, SLOT(OnRadioBalanceWhiteAuto(int)),
            Qt::UniqueConnection);
    }

    m_buttonReset = new QPushButton("Reset all", this);
    m_buttonReset->setFixedSize(80, 24);
    connect(m_buttonReset, SIGNAL(clicked()), this, SLOT(OnButtonReset()), Qt::UniqueConnection);

    m_labelSkipFrames = new QLabel("Damping (skip frames)", this);
    m_labelSkipFrames->setFixedHeight(24);

    m_spinBoxSkipFrames = new QSpinBox(this);
    m_spinBoxSkipFrames->setFixedHeight(24);
    m_spinBoxSkipFrames->setToolTip(
        "Damping value from 1 to 10. Set higher values to avoid oscillation.\n\n"
        "1: skip one frame, calculate and set new image parameters for every second frame.\n"
        "2: skip two frames, calculate and set new image parameters for every third frame.\n"
        "...");
    m_spinBoxSkipFrames->setRange(1, 10);
    connect(m_spinBoxSkipFrames, SIGNAL(valueChanged(int)), this, SLOT(OnSpinBoxSkipFrames(int)), Qt::UniqueConnection);

    m_layoutRadioButtonsExposureAuto = new QHBoxLayout();
    m_layoutRadioButtonsExposureAuto->setSpacing(10);
    m_layoutRadioButtonsExposureAuto->setMargin(0);
    m_layoutRadioButtonsExposureAuto->addWidget(m_labelExposureAutoOff);
    m_layoutRadioButtonsExposureAuto->addWidget(m_radioExposureAutoOff);
    m_layoutRadioButtonsExposureAuto->addStretch(0);
    m_layoutRadioButtonsExposureAuto->addWidget(m_labelExposureAutoOnce);
    m_layoutRadioButtonsExposureAuto->addWidget(m_radioExposureAutoOnce);
    m_layoutRadioButtonsExposureAuto->addStretch(0);
    m_layoutRadioButtonsExposureAuto->addWidget(m_labelExposureAutoContinuous);
    m_layoutRadioButtonsExposureAuto->addWidget(m_radioExposureAutoContinuous);

    if (m_gainAutoSupported)
    {
        m_layoutRadioButtonsGainAuto = new QHBoxLayout();
        m_layoutRadioButtonsGainAuto->setSpacing(10);
        m_layoutRadioButtonsGainAuto->setMargin(0);
        m_layoutRadioButtonsGainAuto->addWidget(m_labelGainAutoOff);
        m_layoutRadioButtonsGainAuto->addWidget(m_radioGainAutoOff);
        m_layoutRadioButtonsGainAuto->addStretch(0);
        m_layoutRadioButtonsGainAuto->addWidget(m_labelGainAutoOnce);
        m_layoutRadioButtonsGainAuto->addWidget(m_radioGainAutoOnce);
        m_layoutRadioButtonsGainAuto->addStretch(0);
        m_layoutRadioButtonsGainAuto->addWidget(m_labelGainAutoContinuous);
        m_layoutRadioButtonsGainAuto->addWidget(m_radioGainAutoContinuous);
    }

    if (m_balanceWhiteAutoSupported)
    {
        m_layoutRadioButtonsBalanceWhiteAuto = new QHBoxLayout();
        m_layoutRadioButtonsBalanceWhiteAuto->setSpacing(10);
        m_layoutRadioButtonsBalanceWhiteAuto->setMargin(0);
        m_layoutRadioButtonsBalanceWhiteAuto->addWidget(m_labelBalanceWhiteAutoOff);
        m_layoutRadioButtonsBalanceWhiteAuto->addWidget(m_radioBalanceWhiteAutoOff);
        m_layoutRadioButtonsBalanceWhiteAuto->addStretch(0);
        m_layoutRadioButtonsBalanceWhiteAuto->addWidget(m_labelBalanceWhiteAutoOnce);
        m_layoutRadioButtonsBalanceWhiteAuto->addWidget(m_radioBalanceWhiteAutoOnce);
        m_layoutRadioButtonsBalanceWhiteAuto->addStretch(0);
        m_layoutRadioButtonsBalanceWhiteAuto->addWidget(m_labelBalanceWhiteAutoContinuous);
        m_layoutRadioButtonsBalanceWhiteAuto->addWidget(m_radioBalanceWhiteAutoContinuous);
    }

    m_layoutSkipFramesControls = new QHBoxLayout();
    m_layoutSkipFramesControls->setSpacing(0);
    m_layoutSkipFramesControls->setMargin(0);
    m_layoutSkipFramesControls->addWidget(m_spinBoxSkipFrames, 1);
    m_layoutSkipFramesControls->addStretch(2);

    m_layoutControls = new QGridLayout();
    m_layoutControls->setVerticalSpacing(5);
    m_layoutControls->setMargin(0);
    m_layoutControls->setColumnStretch(0, 0);
    m_layoutControls->setColumnStretch(1, 1);
    m_layoutControls->setColumnStretch(2, 5);
    m_layoutControls->setColumnStretch(3, 1);
    m_layoutControls->setColumnStretch(4, 0);

    m_layoutControls->addWidget(m_labelExposureAuto, 0, 0);
    m_layoutControls->addLayout(m_layoutRadioButtonsExposureAuto, 0, 2);
    m_layoutControls->addWidget(m_buttonReset, 0, 4);

    if (m_gainAutoSupported)
    {
        m_layoutControls->addWidget(m_labelGainAuto, 1, 0);
        m_layoutControls->addLayout(m_layoutRadioButtonsGainAuto, 1, 2);
    }

    if (m_balanceWhiteAutoSupported)
    {
        m_layoutControls->addWidget(m_labelBalanceWhiteAuto, 2, 0);
        m_layoutControls->addLayout(m_layoutRadioButtonsBalanceWhiteAuto, 2, 2);
    }

    m_layoutControls->addWidget(m_labelSkipFrames, 3, 0);
    m_layoutControls->addLayout(m_layoutSkipFramesControls, 3, 2);

    m_layout->addWidget(m_statusBar);
    m_layout->addSpacing(20);
    m_layout->addLayout(m_layoutControls);
    m_layout->addSpacing(10);
}


void MainWindow::OnCounterUpdated(unsigned int frameCounter, unsigned int errorCounter)
{
    QString strText;
    strText.sprintf("Framerate set to: %.1f, acquired: %d, errors: %d", m_frameRateSet, frameCounter, errorCounter);
    m_labelInfo->setText(strText);
}


void MainWindow::OnAboutQtLinkActivated(const QString& link)
{
    if (link == "#aboutQt")
    {
        QMessageBox::aboutQt(this, "About Qt");
    }
}


void MainWindow::OnRadioExposureAuto(int mode)
{
    m_autoFeaturesManager.SetExposureAutoMode(static_cast<AutoFeaturesManager::ExposureAutoMode>(mode));
    UpdateAutoControls();
}


void MainWindow::OnRadioGainAuto(int mode)
{
    m_autoFeaturesManager.SetGainAutoMode(static_cast<AutoFeaturesManager::GainAutoMode>(mode));
    UpdateAutoControls();
}


void MainWindow::OnRadioBalanceWhiteAuto(int mode)
{
    m_autoFeaturesManager.SetBalanceWhiteAutoMode(static_cast<AutoFeaturesManager::BalanceWhiteAutoMode>(mode));
    UpdateAutoControls();
}


void MainWindow::OnButtonReset()
{
    m_autoFeaturesManager.SetExposureAutoMode(AutoFeaturesManager::ExposureAutoMode::Off);
    m_autoFeaturesManager.SetGainAutoMode(AutoFeaturesManager::GainAutoMode::Off);
    m_autoFeaturesManager.SetBalanceWhiteAutoMode(AutoFeaturesManager::BalanceWhiteAutoMode::Off);
    m_autoFeaturesManager.ResetSkipFramesCounter();

    while (m_autoFeaturesManager.IsCalculating())
    {
        QThread::msleep(100);
    }

    UpdateAutoControls();
}


void MainWindow::OnSpinBoxSkipFrames(int skipFrames)
{
    m_autoFeaturesManager.SetSkipFrames(skipFrames);
    UpdateAutoControls();
}


void MainWindow::UpdateAutoControls()
{
    AutoFeaturesManager::ExposureAutoMode exposureAutoMode = m_autoFeaturesManager.GetExposureAutoMode();
    AutoFeaturesManager::GainAutoMode gainAutoMode = m_autoFeaturesManager.GetGainAutoMode();
    AutoFeaturesManager::BalanceWhiteAutoMode balanceWhiteAutoMode = m_autoFeaturesManager.GetBalanceWhiteAutoMode();

    // Update ExposureAuto controls
    disconnect(m_groupExposureAuto, SIGNAL(buttonClicked(int)), this, SLOT(OnRadioExposureAuto(int)));

    m_radioExposureAutoOff->setChecked((AutoFeaturesManager::ExposureAutoMode::Off == exposureAutoMode));
    m_radioExposureAutoOff->setEnabled(!(AutoFeaturesManager::ExposureAutoMode::Once == exposureAutoMode));
    m_radioExposureAutoOnce->setChecked((AutoFeaturesManager::ExposureAutoMode::Once == exposureAutoMode));
    m_radioExposureAutoOnce->setEnabled(!(AutoFeaturesManager::ExposureAutoMode::Once == exposureAutoMode));
    m_radioExposureAutoContinuous->setChecked((AutoFeaturesManager::ExposureAutoMode::Continuous == exposureAutoMode));
    m_radioExposureAutoContinuous->setEnabled(!(AutoFeaturesManager::ExposureAutoMode::Once == exposureAutoMode));

    connect(
        m_groupExposureAuto, SIGNAL(buttonClicked(int)), this, SLOT(OnRadioExposureAuto(int)), Qt::UniqueConnection);

    // Update GainAuto controls
    if (m_gainAutoSupported)
    {
        disconnect(m_groupGainAuto, SIGNAL(buttonClicked(int)), this, SLOT(OnRadioGainAuto(int)));

        m_radioGainAutoOff->setChecked((AutoFeaturesManager::GainAutoMode::Off == gainAutoMode));
        m_radioGainAutoOff->setEnabled(!(AutoFeaturesManager::GainAutoMode::Once == gainAutoMode));
        m_radioGainAutoOnce->setChecked((AutoFeaturesManager::GainAutoMode::Once == gainAutoMode));
        m_radioGainAutoOnce->setEnabled(!(AutoFeaturesManager::GainAutoMode::Once == gainAutoMode));
        m_radioGainAutoContinuous->setChecked((AutoFeaturesManager::GainAutoMode::Continuous == gainAutoMode));
        m_radioGainAutoContinuous->setEnabled(!(AutoFeaturesManager::GainAutoMode::Once == gainAutoMode));

        connect(m_groupGainAuto, SIGNAL(buttonClicked(int)), this, SLOT(OnRadioGainAuto(int)), Qt::UniqueConnection);
    }

    // Update BalanceWhiteAuto controls
    if (m_balanceWhiteAutoSupported)
    {
        disconnect(m_groupBalanceWhiteAuto, SIGNAL(buttonClicked(int)), this, SLOT(OnRadioBalanceWhiteAuto(int)));

        m_radioBalanceWhiteAutoOff->setChecked(
            (AutoFeaturesManager::BalanceWhiteAutoMode::Off == balanceWhiteAutoMode));
        m_radioBalanceWhiteAutoOff->setEnabled(
            !(AutoFeaturesManager::BalanceWhiteAutoMode::Once == balanceWhiteAutoMode));
        m_radioBalanceWhiteAutoOnce->setChecked(
            (AutoFeaturesManager::BalanceWhiteAutoMode::Once == balanceWhiteAutoMode));
        m_radioBalanceWhiteAutoOnce->setEnabled(
            !(AutoFeaturesManager::BalanceWhiteAutoMode::Once == balanceWhiteAutoMode));
        m_radioBalanceWhiteAutoContinuous->setChecked(
            (AutoFeaturesManager::BalanceWhiteAutoMode::Continuous == balanceWhiteAutoMode));
        m_radioBalanceWhiteAutoContinuous->setEnabled(
            !(AutoFeaturesManager::BalanceWhiteAutoMode::Once == balanceWhiteAutoMode));

        connect(m_groupBalanceWhiteAuto, SIGNAL(buttonClicked(int)), this, SLOT(OnRadioBalanceWhiteAuto(int)),
            Qt::UniqueConnection);
    }

    disconnect(m_spinBoxSkipFrames, SIGNAL(valueChanged(int)), this, SLOT(OnSpinBoxSkipFrames(int)));
    m_spinBoxSkipFrames->setValue(m_autoFeaturesManager.GetSkipFrames());
    connect(m_spinBoxSkipFrames, SIGNAL(valueChanged(int)), this, SLOT(OnSpinBoxSkipFrames(int)), Qt::UniqueConnection);
}
