/*!
 * \file    backend.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2021-03-24
 * \since   1.0.0
 *
 * \version 1.1.1
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

#include "backend.h"

#include "imageitem.h"

#define VERSION "1.1.2"


BackEnd::BackEnd(QObject* parent)
    : QObject(parent)
{
    // initialize peak library
    peak::Library::Initialize();

    // Create worker thread that waits for new images from the camera
    m_acquisitionWorker = new AcquisitionWorker();
    m_acquisitionWorker->moveToThread(&m_acquisitionThread);

    // worker must be started, when the acquisition starts, and deleted, when the worker thread finishes
    connect(&m_acquisitionThread, &QThread::started, m_acquisitionWorker, &AcquisitionWorker::Start);
    connect(&m_acquisitionThread, &QThread::finished, m_acquisitionWorker, &QObject::deleteLater);

    // connect the worker new image signal with the corresponding backend signal
    connect(m_acquisitionWorker, &AcquisitionWorker::imageReceived, this, &BackEnd::imageChanged);

    // Connect the worker counter updated signal with with the corresponding backend signal
    connect(m_acquisitionWorker, &AcquisitionWorker::counterUpdated, this, &BackEnd::counterChanged);

    // Connect the signal from the acquisition worker when an exception was thrown and a message should be printed
    // with the messagebox trigger slot in the BackEnd class
    connect(m_acquisitionWorker, &AcquisitionWorker::messageBoxTrigger, this, &BackEnd::messageBoxTrigger);
}

BackEnd::~BackEnd()
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

void BackEnd::OpenDevice()
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
            qDebug() << "[BackEnd::OpenDevice] ERROR: No device found";
            emit messageBoxTrigger("Error", "No device found.");
            return;
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
                qDebug() << "[BackEnd::OpenDevice] ERROR: Device(s) could not be opened";
                emit messageBoxTrigger("Error", "Device could not be openend");
                return;
            }
        }


        if (m_device)
        {
            // Open standard data stream
            m_dataStream = m_device->DataStreams().at(0)->OpenDataStream();

            // Get nodemap of remote device for all accesses to the genicam nodemap tree
            m_nodemapRemoteDevice = m_device->RemoteDevice()->NodeMaps().at(0);

            // To prepare for untriggered continuous image acquisition, load the default user set if available
            // and wait until execution is finished
            try
            {
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("UserSetSelector")
                    ->SetCurrentEntry("Default");
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->Execute();
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->WaitUntilDone();
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

            // configure worker
            m_acquisitionWorker->SetDataStream(m_dataStream);
            emit acquisitionStarted();
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "[BackEnd::StartAcquisition] EXCEPTION: " << e.what();
        emit messageBoxTrigger("Exception", e.what());
        return;
    }

    // Start thread execution
    m_acquisitionThread.start();
}

void BackEnd::CloseDevice()
{
    m_acquisitionWorker->Stop();

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
            qDebug() << "[BackEnd::stopAcquisition] EXCEPTION: " << e.what();
            emit messageBoxTrigger("Exception", e.what());
        }
    }

    // if data stream was opened, try to stop it and revoke its image buffers
    if (m_dataStream)
    {
        try
        {
            m_dataStream->KillWait(); //->KillOneWait();
            m_dataStream->StopAcquisition(peak::core::AcquisitionStopMode::Default);
            m_dataStream->Flush(peak::core::DataStreamFlushMode::DiscardAll);

            for (const auto& buffer : m_dataStream->AnnouncedBuffers())
            {
                m_dataStream->RevokeBuffer(buffer);
            }
        }
        catch (const std::exception& e)
        {
            qDebug() << "[BackEnd::StopAcquisition] EXCEPTION: " << e.what();
            emit messageBoxTrigger("Exception", e.what());
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
            qDebug() << "[BackEnd::StopAcquisition] EXCEPTION: " << e.what();
            emit messageBoxTrigger("Exception", e.what());
        }
    }
}

QString BackEnd::Version() const
{
    return VERSION;
}

QString BackEnd::QtVersion() const
{
    return qVersion();
}
