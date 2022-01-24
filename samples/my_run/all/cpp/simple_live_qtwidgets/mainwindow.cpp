/*!
 * \file    mainwindow.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2019-05-01
 * \since   1.0.0
 *
 * \version 1.1.0
 *
 * Copyright (C) 2019 - 2021, IDS Imaging Development Systems GmbH.
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

#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QThread>
#include <QWidget>

#include <cstdint>

#define VERSION "1.1.1"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    QWidget* widget = new QWidget(this);
    m_layout = new QVBoxLayout;
    widget->setLayout(m_layout);
    setCentralWidget(widget);

    m_device = nullptr;
    m_display = nullptr;
    m_dataStream = nullptr;
    m_acquisitionWorker = nullptr;


    // initialize peak library
    peak::Library::Initialize();

    if (OpenDevice())
    {
        try
        {
            // Create a display for the camera image
            m_display = new Display(widget);
            m_layout->addWidget(m_display);

            // Create worker thread that waits for new images from the camera
            m_acquisitionWorker = new AcquisitionWorker();
            m_acquisitionWorker->SetDataStream(m_dataStream);
            m_acquisitionWorker->moveToThread(&m_acquisitionThread);

            // worker must be started, when the acquisition starts, and deleted, when the worker thread finishes
            connect(&m_acquisitionThread, &QThread::started, m_acquisitionWorker, &AcquisitionWorker::Start);
            connect(&m_acquisitionThread, &QThread::finished, m_acquisitionWorker, &QObject::deleteLater);

            // Connect the signal from the worker thread when a new image was received with the display update slot in
            // the Display class
            connect(m_acquisitionWorker, &AcquisitionWorker::imageReceived, m_display, &Display::onImageReceived);

            // Connect the signal from the worker thread when the counters have changed with the update slot in the
            // MainWindow class
            connect(m_acquisitionWorker, &AcquisitionWorker::counterUpdated, this, &MainWindow::onCounterUpdated);

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

    createStatusBar();

    // Set minimum window size
    this->setMinimumSize(700, 500);
}


MainWindow::~MainWindow()
{
    DestroyAll();
}


void MainWindow::DestroyAll()
{
    if (m_acquisitionWorker)
    {
        m_acquisitionWorker->Stop();
        m_acquisitionThread.quit();
        m_acquisitionThread.wait();
    }

    CloseDevice();

    // close peak library
    peak::Library::Close();
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

	if (m_nodemapRemoteDevice)
	{
		try
		{
			// Unlock parameters after acquisition stop
			m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("TLParamsLocked")->SetValue(0);
		}
		catch (const std::exception& e)
		{
			QMessageBox::information(this, "Exception", e.what(), QMessageBox::Ok);
		}
	}
}


void MainWindow::createStatusBar()
{
    QWidget* statusBar = new QWidget(centralWidget());
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);

    m_labelInfo = new QLabel(statusBar);
    m_labelInfo->setAlignment(Qt::AlignLeft);
    layout->addWidget(m_labelInfo);
    layout->addStretch();

    QLabel* m_labelVersion = new QLabel(statusBar);
    m_labelVersion->setText(("simple_live_qtwidgets v" VERSION));
    m_labelVersion->setAlignment(Qt::AlignRight);
    layout->addWidget(m_labelVersion);

    QLabel* m_labelAboutQt = new QLabel(statusBar);
    m_labelAboutQt->setObjectName("aboutQt");
    m_labelAboutQt->setText(R"(<a href="#aboutQt">About Qt</a>)");
    m_labelAboutQt->setAlignment(Qt::AlignRight);
    connect(m_labelAboutQt, SIGNAL(linkActivated(QString)), this, SLOT(on_aboutQt_linkActivated(QString)));
    layout->addWidget(m_labelAboutQt);
    statusBar->setLayout(layout);

    m_layout->addWidget(statusBar);
}

void MainWindow::onCounterUpdated(unsigned int frameCounter, unsigned int errorCounter)
{
    QString strText;
    strText.sprintf("Acquired: %d, errors: %d", frameCounter, errorCounter);
    m_labelInfo->setText(strText);
}

void MainWindow::on_aboutQt_linkActivated(const QString& link)
{
    if (link == "#aboutQt")
    {
        QMessageBox::aboutQt(this, "About Qt");
    }
}
