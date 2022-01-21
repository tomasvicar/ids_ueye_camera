/*!
 * \file    camera.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \brief   The Camera class is used to configure, start and stop the camera's
 *          image acquisition.
 *
 * \version 1.0.0
 *
 * Copyright (C) 2020 - 2021, IDS Imaging Development Systems GmbH.
 *
 * The information in this document is subject to change without
 * notice and should not be construed as a commitment by IDS Imaging Development Systems GmbH.
 * IDS Imaging Development Systems GmbH does not assume any responsibility
 * for any errors that may appear in this document.
 *
 * This document, or source code, is provided solely as an example
 * of how to utilize IDS Imaging Development Systems GmbH software libraries in a sample application.
 * IDS Imaging Development Systems GmbH does not assume any responsibility
 * for the use or reliability of any portion of this document.
 *
 * General permission to copy or modify is hereby granted.
 */

#include "camera.h"

#include <QDebug>

#include <algorithm>

Camera::Camera(std::shared_ptr<peak::core::DeviceDescriptor> deviceDescriptor)
{
    m_device = deviceDescriptor->OpenDevice(peak::core::DeviceAccessType::Control);

    // open standard data stream
    m_dataStream = m_device->DataStreams().at(0)->OpenDataStream();

    // get nodemap of remote device for all accesses to the genicam nodemap tree
    m_nodeMapRemoteDevice = m_device->RemoteDevice()->NodeMaps().at(0);

    // the GUI accessible node list
    m_nodeList = std::make_shared<NodeListModel>();

    // create worker thread that waits for new images from the camera
    m_acquisitionWorker = new AcquisitionWorker();
    m_acquisitionThread = new QThread();
    m_acquisitionWorker->moveToThread(m_acquisitionThread);
    m_acquisitionWorker->SetDataStream(m_dataStream);

    // Create worker thread for image conversion from buffer to image object
    m_converterWorker = new ImageConverterWorker();
    m_converterThread = new QThread();
    m_converterWorker->moveToThread(m_converterThread);
    m_converterWorker->setDataStream(m_dataStream);
    m_converterWorker->setImageCount(2);

    // worker must be started, when the acquisition starts
    connect(m_acquisitionThread, &QThread::started, m_acquisitionWorker, &AcquisitionWorker::Start);

    // connect the worker new image signal with the corresponding slot of the camera object
    connect(m_acquisitionWorker, &AcquisitionWorker::bufferReceived, m_converterWorker, &ImageConverterWorker::convert);
    connect(m_converterWorker, &ImageConverterWorker::imageReceived, this, &Camera::imageReceived);

    // connect the worker counter updated signal with the corresponding slot of the camera object
    connect(m_acquisitionWorker, &AcquisitionWorker::counterUpdated, this, &Camera::onCounterReceived);

    m_acquisitionStarted = false;
    m_converterThread->start();
}

void Camera::close()
{
    // stop acquisition if it is still running
    if (m_acquisitionStarted)
    {
        stopAcquisition();
    }

    // stop acquisition worker and thread
    m_acquisitionWorker->Stop();
    delete m_acquisitionWorker;
    m_acquisitionWorker = nullptr;

    m_acquisitionThread->quit();
    m_acquisitionThread->wait();
    delete m_acquisitionThread;
    m_acquisitionThread = nullptr;

    // stop converter worker and thread
    delete m_converterWorker;
    m_converterWorker = nullptr;

    m_converterThread->quit();
    m_converterThread->wait();
    delete m_converterThread;
    m_converterThread = nullptr;

    // stop updating the nodeList
    m_nodeList->stopUpdating();

    // clean up device and datastream
    if (m_device)
    {
        m_dataStream = nullptr;
        m_device = nullptr;
    }
    else
    {
        qDebug() << "[Camera::~Camera] Nothing to do";
    }
}

std::string Camera::key()
{
    return m_device->Key();
}

std::shared_ptr<NodeListModel> Camera::nodeList()
{
    return m_nodeList;
}

std::shared_ptr<peak::core::NodeMap> Camera::nodeMapRemoteDevice() const
{
    return m_nodeMapRemoteDevice;
}

void Camera::setBufferCountMinSequencer(int bufferCountMinSequencer)
{
    // set the minimum number of buffers for sequencer acquisition, e.g. 2 x sequence count
    m_bufferCountMinSequencer = bufferCountMinSequencer;
    m_converterWorker->setImageCount(bufferCountMinSequencer);
}

void Camera::loadUserSet(std::string userSet)
{
    try
    {
        m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("UserSetSelector")
            ->SetCurrentEntry(userSet);
        m_nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->Execute();
        m_nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->WaitUntilDone();
    }
    catch (const std::exception& e)
    {
        qDebug() << "[Camera::prepareFreerunAcquisition] EXCEPTION: " << e.what();
        return;
    }
}

void Camera::prepareAcquisition(AcquisitionType type, int param)
{
    try
    {
        switch (type)
        {
        case AcquisitionType::ContinousPwmTrigger:
        {
            // configure the image acquisition to triggered mode with contiuous framerate
            // using the PWM to generate a continuous signal with the framerate given in 'param'

            // configure the PWM to be 'always on'
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("PWMSelector")
                ->SetCurrentEntry("PWM0");
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("PWMTriggerSource")
                ->SetCurrentEntry("UserOutput0");
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("PWMTriggerActivation")
                ->SetCurrentEntry("LevelLow");

            // set the given framerate (or the maximum framerate that can be transmitted) as PWMFrequency
            auto maxFramerateInterface = m_nodeMapRemoteDevice
                                             ->FindNode<peak::core::nodes::FloatNode>(
                                                 "DeviceLinkAcquisitionFrameRateLimit")
                                             ->Value();
            auto framerate = std::min(maxFramerateInterface, static_cast<double>(param));
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("PWMFrequency")->SetValue(framerate);

            // configure the ExposureStart trigger to start at each rising edge of the PWM
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                ->SetCurrentEntry("ExposureStart");
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                ->SetCurrentEntry("On");
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSource")
                ->SetCurrentEntry("PWM0");
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerActivation")
                ->SetCurrentEntry("RisingEdge");

            break;
        }
        case AcquisitionType::CountedBurstTrigger:
        {
            // configure the the image acquisition to triggered mode with an image burst of N images as fast as possible
            // using a counter as trigger signal and as limiter for the image burst of N ('param')

            // configure the counter to increment at each ExposureStart, stopping at 'param'
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("CounterSelector")
                ->SetCurrentEntry("Counter0");
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("CounterEventSource")
                ->SetCurrentEntry("ExposureStart");
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("CounterResetSource")
                ->SetCurrentEntry("Off");
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("CounterTriggerSource")
                ->SetCurrentEntry("Off");
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("CounterDuration")
                ->SetValue(param);

            // configure the ExposureStart trigger to start immediately when ready, als long as the counter has not
            // finished yet
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                ->SetCurrentEntry("ExposureStart");
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                ->SetCurrentEntry("On");
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSource")
                ->SetCurrentEntry("Counter0Active");
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerActivation")
                ->SetCurrentEntry("LevelHigh");

            // set the software trigger to target at the CounterReset command
            m_triggerSelectorNode = m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>(
                "CounterSelector");
            m_triggerSelectorEntry = "Counter0";
            m_triggerNode = m_nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("CounterReset");
            break;
        }
        case AcquisitionType::SoftwareTrigger:
        {
            // configure the the image acquisition to triggered mode with each single image triggered separately
            // using the software trigger

            // configure the ExposureStart trigger to start with the software trigger
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                ->SetCurrentEntry("ExposureStart");
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                ->SetCurrentEntry("On");
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSource")
                ->SetCurrentEntry("Software");

            // set the software trigger to target at the TriggerSoftware command
            m_triggerSelectorNode = m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>(
                "TriggerSelector");
            m_triggerSelectorEntry = "ExposureStart";
            m_triggerNode = m_nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("TriggerSoftware");

            break;
        }
        default:
        {
            // configure the the image acquisition to freerun mode as default
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                ->SetCurrentEntry("ExposureStart");
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                ->SetCurrentEntry("Off");
            break;
        }
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "[Camera::prepareFreerunAcquisition] EXCEPTION: " << e.what();
        return;
    }
}

bool Camera::startAcquisition()
{
    // check if the acquisition is not already started
    if (m_acquisitionStarted == false)
    {
        try
        {
            // get the payload size for correct buffer allocation
            auto payloadSize = m_nodeMapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")
                                   ->Value();

            // get the minimum number of buffers that must be announced and check that it is not smaller than
            // the minimum buffer count for sequencer acquisition
            auto bufferCountMinDataStream = m_dataStream->NumBuffersAnnouncedMinRequired();
            auto bufferCountMax = std::max(bufferCountMinDataStream, static_cast<size_t>(m_bufferCountMinSequencer));

            // allocate and announce image buffers and queue them
            for (size_t bufferCount = 0; bufferCount < bufferCountMax; ++bufferCount)
            {
                auto buffer = m_dataStream->AllocAndAnnounceBuffer(static_cast<size_t>(payloadSize), nullptr);
                m_dataStream->QueueBuffer(buffer);
            }
            m_nodeList->resumeUpdating();

            // Lock critical features to prevent them from changing during acquisition
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("TLParamsLocked")->SetValue(1);

            // start acquisition
            m_dataStream->StartAcquisition();
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->Execute();
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")
                ->WaitUntilDone();
            qDebug() << "[Camera::startAcquisition] Device started";

            // start the acquisition worker
            if (m_acquisitionWorker->currentState() == AcquisitionWorker::IDLE)
            {
                m_acquisitionThread->start();
            }
            else if (m_acquisitionWorker->currentState() == AcquisitionWorker::PAUSED)
            {
                m_acquisitionWorker->Resume();
            }

            m_acquisitionStarted = true;
            return m_acquisitionStarted;
        }
        catch (const std::exception& e)
        {
            qDebug() << "[Camera::startAcquisition] EXCEPTION: " << e.what();
            return m_acquisitionStarted;
        }
    }
    qDebug() << "[Camera::startAcquisition] Start Acquisition was successfull...";
    return m_acquisitionStarted;
}

bool Camera::stopAcquisition()
{
    // check if the acquisition is running
    if (m_acquisitionStarted == true)
    {
        // pause the acquisitionWorker
        if (m_acquisitionWorker->currentState() == AcquisitionWorker::WorkerState::RUNNING)
        {
            m_acquisitionWorker->Pause();
            m_dataStream->KillWait(); //->KillOneWait();
        }

        // if device was opened, try to stop acquisition
        if (m_device)
        {
            try
            {
                m_nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStop")->Execute();
                m_nodeMapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStop")
                    ->WaitUntilDone();
                qDebug() << "[Camera::stopAcquisition] Device stopped";
            }
            catch (const std::exception& e)
            {
                qDebug() << "[BackEnd::stopAcquisition] EXCEPTION: " << e.what();
            }
        }

        // if data stream was opened, try to stop it and revoke its image buffers
        if (m_dataStream)
        {
            try
            {
                m_dataStream->StopAcquisition(peak::core::AcquisitionStopMode::Kill);
                m_dataStream->Flush(peak::core::DataStreamFlushMode::DiscardAll);

                for (const auto& buffer : m_dataStream->AnnouncedBuffers())
                {
                    m_dataStream->RevokeBuffer(buffer);
                }
            }
            catch (const std::exception& e)
            {
                qDebug() << "[Camera::StopAcquisition] EXCEPTION: " << e.what();
            }
        }

        try
        {
            // Unlock parameters after acquisition stop
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("TLParamsLocked")->SetValue(0);
        }
        catch (const std::exception& e)
        {
            qDebug() << "[Camera::StopAcquisition] EXCEPTION: " << e.what();
        }

        m_acquisitionStarted = false;
        return m_acquisitionStarted;
    }
    else
    {
        return m_acquisitionStarted;
    }
}

void Camera::enableSequencerConfiguration(bool mode)
{
    try
    {
        if (mode == true)
        {
            // enable sequencer configuration mode
            auto node = m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>(
                "SequencerConfigurationMode");
            node->SetCurrentEntry(node->FindEntry("On"));
        }
        else
        {
            // disable sequencer configuration mode
            auto node = m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>(
                "SequencerConfigurationMode");
            node->SetCurrentEntry(node->FindEntry("Off"));
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "[Camera::enableSequencerConfiguration] EXCEPTION: " << e.what();
    }
}

void Camera::enableSequencer(bool mode)
{
    try
    {
        if (mode == true)
        {
            // enable sequencer mode
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("SequencerMode")
                ->SetCurrentEntry("On");
            m_nodeList->pauseUpdating();
        }
        else
        {
            // disable sequencer mode
            m_nodeMapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("SequencerMode")
                ->SetCurrentEntry("Off");
            m_nodeList->resumeUpdating();
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "[Camera::enableSequencer] EXCEPTION: " << e.what();
    }
}

void Camera::onCounterReceived(const unsigned int imageCounter, const unsigned int errorCounter)
{
    emit counterReceived(imageCounter, errorCounter);
}

void Camera::softwareTrigger()
{
    // do a software trigger on the selected trigger node
    m_triggerSelectorNode->SetCurrentEntry(m_triggerSelectorEntry);
    m_triggerNode->Execute();
}

void Camera::updateNodeList()
{
    // update the complete node list
    m_nodeList->setNodeMap(m_nodeMapRemoteDevice);
    emit nodeListChanged();
}
