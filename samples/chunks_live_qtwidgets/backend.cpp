/*!
 * \file    backend.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \version 1.0.2
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

#include "backend.h"
#include <QDebug>

BackEnd::BackEnd(QObject* parent) : QObject(parent)
{
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

    // Connect the signal from the acquisition worker when an exception was thrown and a message should be printed
    // with the messagebox trigger slot in the BackEnd class
    connect(m_acquisitionWorker, &AcquisitionWorker::messageBoxTrigger, this, &BackEnd::messageBoxTrigger);

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
        return false;
    }

    // Start thread execution
    m_acquisitionThread.start();

    return true;
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

            // To prepare for untriggered continuous image acquisition, load the default user set if available
            // and wait until execution is finished
            try
            {
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("UserSetSelector")->SetCurrentEntry("Default");

                m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->Execute();
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->WaitUntilDone();
            }
            catch (const peak::core::NotFoundException&)
            {
                // UserSet is not available
            }

            try
            {
                // Set auto exposure continuous
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("ExposureAuto")->SetCurrentEntry("Continuous");
            }
            catch (const peak::core::NotFoundException&)
            {
                // Auto exposure is not available
            }

            try
            {
                // Activate the inclusion of the chunk data in the payload of the image
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::BooleanNode>("ChunkModeActive")->SetValue(true);

                // The width, height and pixelformat chunks must be activated for the exposuretime chunk to work
                // Select and enable the width chunk
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("ChunkSelector")->SetCurrentEntry("Width");
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::BooleanNode>("ChunkEnable")->SetValue(true);

                // Select and enable the height chunk
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("ChunkSelector")->SetCurrentEntry("Height");
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::BooleanNode>("ChunkEnable")->SetValue(true);

                // Select and enable the pixel format chunk
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("ChunkSelector")->SetCurrentEntry("PixelFormat");
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::BooleanNode>("ChunkEnable")->SetValue(true);

                // Select and enable the exposure time chunk
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("ChunkSelector")->SetCurrentEntry("ExposureTime");
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::BooleanNode>("ChunkEnable")->SetValue(true);
            }
            catch (const peak::core::NotFoundException&)
            {
				qDebug() << "Exception: The camera does not support ChunkData. "
					"This sample opens the first camera available. If you wanted to use a different camera, try to disconnect all other cameras.";
				emit messageBoxTrigger("Exception", "The camera does not support ChunkData. "
					"This sample opens the first camera available. If you wanted to use a different camera, try to disconnect all other cameras.");
            }
            catch (const std::exception& e)
            {
                qDebug() << "Exception: " << e.what();
                emit messageBoxTrigger("Exception", e.what());
            }

            // Get the payload size for correct buffer allocation
            const auto payloadSize = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")->Value();

            // Get the minimum number of buffers that must be announced
            const auto bufferCountMax = m_dataStream->NumBuffersAnnouncedMinRequired();

            // Allocate and announce image buffers and queue them
            for (size_t bufferCount = 0; bufferCount < bufferCountMax; ++bufferCount)
            {
                auto buffer = m_dataStream->AllocAndAnnounceBuffer(static_cast<size_t>(payloadSize), nullptr);
                m_dataStream->QueueBuffer(buffer);
            }

            // Configure worker
            m_acquisitionWorker->setDataStream(m_dataStream);
            m_acquisitionWorker->setNodemapRemoteDevice(m_nodemapRemoteDevice);
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
            emit messageBoxTrigger("Exception", e.what());
        }
    }

    // If data stream was opened, try to stop it and revoke its image buffers
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
            qDebug() << "Exception: " << e.what();
            emit messageBoxTrigger("Exception", e.what());
        }
    }

    try
    {
        // Unlock parameters after acquisition stop
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("TLParamsLocked")->SetValue(0);
    }
    catch (const std::exception& e)
    {
        qDebug() << "Exception: " << e.what();
        emit messageBoxTrigger("Exception", e.what());
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
