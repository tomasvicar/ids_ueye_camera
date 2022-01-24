/*!
 * \file    backend.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2021-01-15
 * \since   1.2.0
 *
 * \version 1.0.0
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

#include "backend.h"

#include <QDebug>

BackEnd::BackEnd(QObject* parent)
    : QObject(parent)
{
    m_submitted = false;

    // Create acquisition worker thread that waits for new images from the camera
    m_acquisitionWorker = new AcquisitionWorker;
    m_acquisitionWorker->moveToThread(&m_acquisitionThread);

    // Worker must be started, when the acquisition starts, and deleted, when the worker thread finishes
    connect(&m_acquisitionThread, &QThread::started, m_acquisitionWorker, &AcquisitionWorker::start);
    connect(&m_acquisitionThread, &QThread::finished, m_acquisitionWorker, &QObject::deleteLater);

    // Connect the worker new image signal with the corresponding slot of the backend
    connect(m_acquisitionWorker, &AcquisitionWorker::imageReceived, this, &BackEnd::imageReceived);

    // Connect the worker counter updated signal with with the corresponding slot of the backend
    connect(m_acquisitionWorker, &AcquisitionWorker::countersUpdated, this, &BackEnd::countersUpdated);

    // Connect the signal from the acquisition worker and the config dialog when an exception was thrown
    // and a message should be printed with the messagebox trigger slot in the BackEnd class
	connect(m_acquisitionWorker, &AcquisitionWorker::messageBoxTrigger, this, &BackEnd::messageBoxTrigger);
    connect(m_acquisitionWorker, &AcquisitionWorker::errorOccurred, this, &BackEnd::errorOccurred);

    // Tell the acquisition worker if a trigger was executed when software trigger is active
    connect(this, &BackEnd::triggerExecuted, m_acquisitionWorker, &AcquisitionWorker::triggerExecuted);

	// Connect camera disconnect signal
	connect(m_acquisitionWorker, &AcquisitionWorker::cameraDisconnected, this, &BackEnd::cameraDisconnected);

    // Initialize peak library
    peak::Library::Initialize();
}

BackEnd::~BackEnd()
{
    if (m_acquisitionWorker)
    {
        m_acquisitionWorker->stop();
        m_acquisitionThread.quit();
        m_acquisitionThread.wait();
    }

    closeDevice();

    // Close peak library
    peak::Library::Close();
}

bool BackEnd::start()
{
    if (!openDevice())
    {
        emit started(false);
        return false;
    }
    else
    {
        // Set UserSetSelector to 'Linescan' and load UserSet
        try
        {
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("UserSetSelector")
                ->SetCurrentEntry("Linescan");
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->Execute();
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->WaitUntilDone();

            m_configDialog = new ConfigDialog(m_nodemapRemoteDevice, this);
            m_configDialog->show();

			connect(m_configDialog, &ConfigDialog::messageBoxTrigger, this, &BackEnd::messageBoxTrigger);
			connect(m_configDialog, &ConfigDialog::cameraDisconnected, this, &BackEnd::cameraDisconnected);
            connect(m_configDialog, &ConfigDialog::configSubmitted, this, &BackEnd::handleConfigSubmitted);
            connect(m_configDialog, &ConfigDialog::destroyed, this, &BackEnd::handleConfigDialogClosed);
            connect(m_configDialog, &ConfigDialog::dialogRejected, this, &BackEnd::handleConfigDialogClosed);
        }
        catch (const std::exception& e)
        {
            qDebug() << "Error: The camera does not support Linescan. "
                        "This sample opens the first camera available. If you wanted to use a different camera, try to "
                        "disconnect all other cameras."
                     << e.what();
            emit messageBoxTrigger("Error",
                "The camera does not support Linescan. "
                "This sample opens the first camera available. If you wanted to use a different camera, try to "
                "disconnect all other cameras.");
            return false;
        }
    }

    return true;
}

void BackEnd::handleConfigSubmitted(int offsetY, int imageHeight,
    std::pair<bool, std::pair<QString, QString>> frameStartTrigger,
    std::pair<bool, std::pair<QString, QString>> lineStartTrigger, int acquisitionLineRate, int PWMFrequency)
{
    m_frameStartTrigger.first = frameStartTrigger.first;
    m_frameStartTrigger.second.first = frameStartTrigger.second.first;
    m_frameStartTrigger.second.second = frameStartTrigger.second.second;
    m_lineStartTrigger.first = lineStartTrigger.first;
    m_lineStartTrigger.second.first = lineStartTrigger.second.first;
    m_lineStartTrigger.second.second = lineStartTrigger.second.second;

    m_submitted = true;
    m_configDialog->close();

    try
    {
        // Get the payload size for correct buffer allocation
        const auto payloadSize = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")
                                     ->Value();

        // Get the minimum number of buffers that must be announced
        const auto bufferCountMax = m_dataStream->NumBuffersAnnouncedMinRequired();

        // Allocate and announce image buffers and queue them
        for (size_t bufferCount = 0; bufferCount < bufferCountMax; ++bufferCount)
        {
            auto buffer = m_dataStream->AllocAndAnnounceBuffer(static_cast<size_t>(payloadSize), nullptr);
            m_dataStream->QueueBuffer(buffer);
        }
    }
	catch (const peak::core::InternalErrorException& e)
	{
		qDebug() << "Exception: " << e.what();
		emit cameraDisconnected();
		return;
	}
    catch (const std::exception& e)
    {
        qDebug() << "Exception: " << e.what();
        emit messageBoxTrigger("Exception", e.what());
    }

    // Configure worker
    m_acquisitionWorker->setDataStream(m_dataStream);
    m_acquisitionWorker->setNodemapRemoteDevice(m_nodemapRemoteDevice);

    // Start thread execution
    m_acquisitionThread.start();

    emit started(true);
}

bool BackEnd::openDevice()
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
            qDebug() << "Error: No device found";
            emit messageBoxTrigger("Error", "No device found");
            return false;
        }

        // Open the first openable device in the device manager's device list
        size_t deviceCount = deviceManager.Devices().size();
        for (size_t i = 0; i < deviceCount; ++i)
        {
            if (deviceManager.Devices().at(i)->IsOpenable())
            {
                m_device = deviceManager.Devices().at(i)->OpenDevice(peak::core::DeviceAccessType::Control);

                // Stop after the first opened device
                break;
            }
            else if (i == (deviceCount - 1))
            {
                qDebug() << "Error: Device could not be openend";
                emit messageBoxTrigger("Error", "Device could not be openend");
                return false;
            }
        }

        if (m_device)
        {
            // Check if any datastreams are available
            auto dataStreams = m_device->DataStreams();
            if (dataStreams.empty())
            {
                m_device.reset();
                qDebug() << "Error: Device has no DataStream";
                emit messageBoxTrigger("Error", "Device has no DataStream");
                return false;
            }

            // Open standard data stream
            m_dataStream = dataStreams.at(0)->OpenDataStream();

            // Get nodemap of remote device for all accesses to the genicam nodemap tree
            m_nodemapRemoteDevice = m_device->RemoteDevice()->NodeMaps().at(0);
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "Exception: " << e.what();
        emit messageBoxTrigger("Exception", e.what());
        return false;
    }

    return true;
}

void BackEnd::closeDevice()
{
    // If device was opened, try to stop acquisition
    if (m_device)
    {
        try
        {
            auto remoteNodeMap = m_device->RemoteDevice()->NodeMaps().at(0);
            remoteNodeMap->FindNode<peak::core::nodes::CommandNode>("AcquisitionStop")->Execute();
            remoteNodeMap->FindNode<peak::core::nodes::CommandNode>("AcquisitionStop")->WaitUntilDone();
        }
        catch (const std::exception& e)
        {
            qDebug() << "Exception: " << e.what();
            // Camera maybe allready closed
        }
    }

    // If data stream was opened, try to stop it and revoke its image buffers
    if (m_dataStream && m_dataStream->IsGrabbing())
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
            qDebug() << "Exception: " << e.what();
        }
    }
}

void BackEnd::handleConfigDialogClosed()
{
    if (!m_submitted)
    {
        emit started(false);
    }
}

int BackEnd::getImageWidth() const
{
    return m_acquisitionWorker->getImageWidth();
}

int BackEnd::getImageHeight() const
{
    return m_acquisitionWorker->getImageHeight();
}

std::shared_ptr<peak::core::NodeMap> BackEnd::getNodemapRemoteDevice()
{
    return m_nodemapRemoteDevice;
}

std::pair<bool, std::pair<QString, QString>> BackEnd::getFrameStartTrigger()
{
    return m_frameStartTrigger;
}

std::pair<bool, std::pair<QString, QString>> BackEnd::getLineStartTrigger()
{
    return m_lineStartTrigger;
}
