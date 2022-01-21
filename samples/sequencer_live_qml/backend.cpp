/*!
 * \file    backend.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \version 1.0.1
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

#include "backend.h"

#include <peak/library/peak_library.hpp>

#include <QDebug>
#include <QThread>

#define VERSION "1.1.2"


BackEnd::BackEnd(QObject* parent)
    : QObject(parent)
{
    // open IDS peak API
    peak::Library::Initialize();
}

BackEnd::~BackEnd()
{
    // emit signal, that BackEnd is closing
    emit closing();

    // close camera, if it is still open
    closeCamera();

    // close IDS peak API
    try
    {
        peak::Library::Close();
    }
    catch (const std::exception& e)
    {
        qDebug() << "[BackEnd::~BackEnd] EXCEPTION: " << e.what();
    }
    qDebug() << "[BackEnd::~BackEnd]";
}

void BackEnd::init()
{
    // open first available camera
    openCamera();

    if (!m_camera)
    {
        qDebug() << "[BackEnd::init] No camera found. Exiting program ...";
        emit error("No camera found. Exiting program ...");
        return;
    }

    // make sure the camera supports the sequencer by checking that all required nodes are available
    try
    {
        m_nodeMap->FindNode("SequencerMode");
        m_nodeMap->FindNode("SequencerConfigurationMode");
        m_nodeMap->FindNode("SequencerSetSelector");
        m_nodeMap->FindNode("SequencerPathSelector");
        m_nodeMap->FindNode("SequencerSetNext");
        m_nodeMap->FindNode("SequencerTriggerSource");
        m_nodeMap->FindNode("SequencerTriggerActivation");
        m_nodeMap->FindNode("SequencerSetSave");
        m_nodeMap->FindNode("SequencerSetLoad");
    }
    catch (std::exception& e)
    {
        qDebug() << "[BackEnd::init] EXCEPTION: " << e.what();
        emit error("The camera does not support sequencer feature. Make sure the correct camera is connected and try "
                   "to disconnect all other cameras.");
        return;
    }

    // for initial preparation, load default userset
    m_camera->loadUserSet("Default");

    // emit signal that camera has been opened
    emit cameraOpened();

    //    cameraSettingsStartup();
}

void BackEnd::cameraSettingsStartup()
{
    // prepare for continuously triggered acquisition using the pwm as trigger source
    m_camera->prepareAcquisition(AcquisitionType::ContinousPwmTrigger, 25);

    // make sure, the camera object allocates an adequate number of image buffers for sequencer
    m_camera->setBufferCountMinSequencer(m_sequencerDuration * 2);

    // start sequencer configuration
    m_camera->enableSequencerConfiguration(true);

    // prepare 4 (m_sequencerDuration) sequencer sets, that can be configured later
    prepareSequencer(m_sequencerDuration);

    // start image acquisition of the camera
    startAcquisition();
    emit startupFinished();
}

void BackEnd::updateCameraList()
{
    try
    {
        // Create instance of the device manager
        auto& deviceManager = peak::DeviceManager::Instance();

        // Update the device manager
        deviceManager.Update();

        m_devices = deviceManager.Devices();
    }
    catch (std::exception& e)
    {
        qDebug() << "[BackEnd::BackEnd] EXCEPTION: " << e.what();
    }
}

void BackEnd::openCamera(int index)
{
    try
    {
        bool openable = false;

        // if no index was entered, automatically open first openable device
        if (index == -1)
        {
            // search for connected cameras
            updateCameraList();

            // check if the cameras can be opened
            int i = 0;
            for (auto& device : m_devices)
            {
                if (device->IsOpenable())
                {
                    index = i;
                    openable = true;
                    break;
                }
                ++i;
            }
        }
        // if an index was entered, check if the camera can be opened
        else if (m_devices.at(static_cast<unsigned int>(index))->IsOpenable())
        {
            openable = true;
        }

        // if the selected camera cannot be opened or there is no openable camera, return
        if (!openable)
        {
            qDebug() << "[BackEnd::openCamera]: Could not find camera or it was not openable";
            return;
        }

        // open the camera
        m_camera = std::make_shared<Camera>(m_devices.at(static_cast<unsigned int>(index)));
        m_openedCamera = m_camera->key();

        // connect the camera signals to the corresponding signals of the backend
        connect(m_camera.get(), &Camera::imageReceived, this, &BackEnd::imageReceived);
        connect(m_camera.get(), &Camera::nodeListChanged, this, &BackEnd::nodeListChanged);

        // update the nodemap and let the camera object generate a nodelist, usable for GUI interaction
        m_camera->updateNodeList();
        m_nodeMap = m_camera->nodeMapRemoteDevice();
        m_nodeList = m_camera->nodeList();
        emit nodeListChanged();


        qDebug() << "[BackEnd::openCamera] camera opened";
    }
    catch (const std::exception& e)
    {
        qDebug() << "[BackEnd::openCamera] EXCEPTION: " << e.what();
        return;
    }
}

void BackEnd::closeCamera()
{
    // check if there is an opened camera object
    if (m_camera)
    {
        // emit signal that camera is about to close, so that GUI might react to it
        emit cameraAboutToClose();

        // stop image acquisition
        stopAcquisition();

        // clear the node list
        m_nodeList = std::make_shared<NodeListModel>();
        emit nodeListChanged();
        m_nodeMap = nullptr;

        // close the camera
        m_camera->close();
        m_camera = nullptr;

        // emit signal, that camera has been closed
        emit cameraClosed();

        // update the camera list
        updateCameraList();
    }
    else
    {
        qDebug() << "[BackEnd::closeCamera] Nothing to do";
    }
}

void BackEnd::startAcquisition()
{
    // start the image acquisition of the camera
    bool b = m_camera->startAcquisition();

    // if the Q_PROPERTY acquisitionRunning has changed, emit the corresponding signal
    if (b != m_acquisitionRunning)
    {
        m_acquisitionRunning = b;
        emit acquisitionRunningChanged();
    }
}

void BackEnd::stopAcquisition()
{
    // stop the image acquisition of the camera
    bool b = m_camera->stopAcquisition();

    // if the Q_PROPERTY acquisitionRunning has changed, emit the corresponding signal
    if (b != m_acquisitionRunning)
    {
        m_acquisitionRunning = b;
        emit acquisitionRunningChanged();
    }
}

void BackEnd::applyPixelFormat(int index)
{
    // check if the acquisition should be restarted after applying the PixelFormat
    bool restart = false;
    if (m_acquisitionRunning)
    {
        stopAcquisition();
        restart = true;
    }

    // apply PixelFormat
    m_nodeList->get("PixelFormat")->entryListModel()->setCurrentIndex(index);

    // restart acquisition if necessary
    if (restart)
    {
        startAcquisition();
    }
}

void BackEnd::applyRoi(QRect roi)
{
    // check if the acquisition should be restarted after applying the ROI
    bool restart = false;
    if (m_acquisitionRunning)
    {
        stopAcquisition();
        restart = true;
    }

    // apply the ROI, starting with width and height
    m_nodeMap->FindNode<peak::core::nodes::IntegerNode>("Width")->SetValue(roi.width());
    m_nodeMap->FindNode<peak::core::nodes::IntegerNode>("Height")->SetValue(roi.height());
    m_nodeMap->FindNode<peak::core::nodes::IntegerNode>("OffsetX")->SetValue(roi.x());
    m_nodeMap->FindNode<peak::core::nodes::IntegerNode>("OffsetY")->SetValue(roi.y());

    // restart acquisition if necessary
    if (restart)
    {
        startAcquisition();
    }
}

void BackEnd::resetRoi()
{
    // check if the acquisition should be restarted after resetting the ROI
    bool restart = false;
    if (m_acquisitionRunning)
    {
        stopAcquisition();
        restart = true;
    }

    // reset the ROI, starting with the offsets
    m_nodeMap->FindNode<peak::core::nodes::IntegerNode>("OffsetX")->SetValue(0);
    m_nodeMap->FindNode<peak::core::nodes::IntegerNode>("OffsetY")->SetValue(0);
    auto maxWidth = m_nodeMap->FindNode<peak::core::nodes::IntegerNode>("Width")->Maximum();
    m_nodeMap->FindNode<peak::core::nodes::IntegerNode>("Width")->SetValue(maxWidth);
    auto maxHeight = m_nodeMap->FindNode<peak::core::nodes::IntegerNode>("Height")->Maximum();
    m_nodeMap->FindNode<peak::core::nodes::IntegerNode>("Height")->SetValue(maxHeight);

    // restart acquisition if necessary
    if (restart)
    {
        startAcquisition();
    }
}

void BackEnd::prepareSequencer(int sequencerDuration)
{
    try
    {
        // prepare the given number of sequencer sets with some presets
        for (int i = 0; i < sequencerDuration; ++i)
        {
            // configure the sequencer path
            m_nodeMap->FindNode<peak::core::nodes::IntegerNode>("SequencerSetSelector")->SetValue(i);
            m_nodeMap->FindNode<peak::core::nodes::IntegerNode>("SequencerPathSelector")->SetValue(0);
            m_nodeMap->FindNode<peak::core::nodes::IntegerNode>("SequencerSetNext")
                ->SetValue((i + 1) % sequencerDuration);

            // set the sequencer trigger source to the start of each exposure
            m_nodeMap->FindNode<peak::core::nodes::EnumerationNode>("SequencerTriggerSource")
                ->SetCurrentEntry("ExposureStart");
            m_nodeMap->FindNode<peak::core::nodes::EnumerationNode>("SequencerTriggerActivation")
                ->SetCurrentEntry("RisingEdge");

            // save the sequencer set to make sure, the sequencer set is valid
            m_nodeMap->FindNode<peak::core::nodes::CommandNode>("SequencerSetSave")->Execute();
            m_nodeMap->FindNode<peak::core::nodes::CommandNode>("SequencerSetSave")->WaitUntilDone();
        }
        qDebug() << "[BackEnd::setSequencerConfiguration] sequencer configuration finished";

        // Enable Chunks
        m_nodeMap->FindNode<peak::core::nodes::BooleanNode>("ChunkModeActive")->SetValue(true);
        m_nodeMap->FindNode<peak::core::nodes::EnumerationNode>("ChunkSelector")
            ->SetCurrentEntry("SequencerSetActive");
        m_nodeMap->FindNode<peak::core::nodes::BooleanNode>("ChunkEnable")->SetValue(true);

        // For GEV devices, additional chunks must be enabled
        m_nodeMap->FindNode<peak::core::nodes::EnumerationNode>("ChunkSelector")->SetCurrentEntry("Width");
        m_nodeMap->FindNode<peak::core::nodes::BooleanNode>("ChunkEnable")->SetValue(true);
        m_nodeMap->FindNode<peak::core::nodes::EnumerationNode>("ChunkSelector")->SetCurrentEntry("Height");
        m_nodeMap->FindNode<peak::core::nodes::BooleanNode>("ChunkEnable")->SetValue(true);
        m_nodeMap->FindNode<peak::core::nodes::EnumerationNode>("ChunkSelector")
            ->SetCurrentEntry("PixelFormat");
        m_nodeMap->FindNode<peak::core::nodes::BooleanNode>("ChunkEnable")->SetValue(true);

        // for IDS cameras, activate ChunkTimestamp
        const auto vendorName = m_nodeMap->FindNode<peak::core::nodes::StringNode>("DeviceVendorName")
                                    ->Value();
        if (vendorName.find("IDS") != std::string::npos)
        {
            m_nodeMap->FindNode<peak::core::nodes::EnumerationNode>("ChunkSelector")
                ->SetCurrentEntry("Timestamp");
            m_nodeMap->FindNode<peak::core::nodes::BooleanNode>("ChunkEnable")->SetValue(true);
        }
    }
    catch (std::exception e)
    {
        qDebug() << "[BackEnd::prepareSequencer] EXCEPTION: " << e.what();
    }
}

void BackEnd::loadSequencerSet(int index)
{
    try
    {
        m_nodeMap->FindNode<peak::core::nodes::IntegerNode>("SequencerSetSelector")->SetValue(index);
        m_nodeMap->FindNode<peak::core::nodes::CommandNode>("SequencerSetLoad")->Execute();
        m_nodeMap->FindNode<peak::core::nodes::CommandNode>("SequencerSetLoad")->WaitUntilDone();
    }
    catch (std::exception e)
    {
        qDebug() << "[BackEnd::BackEnd] EXCEPTION: " << e.what();
    }
}

void BackEnd::saveSequencerSet(int index)
{
    try
    {
        m_nodeMap->FindNode<peak::core::nodes::IntegerNode>("SequencerSetSelector")->SetValue(index);
        m_nodeMap->FindNode<peak::core::nodes::CommandNode>("SequencerSetSave")->Execute();
        m_nodeMap->FindNode<peak::core::nodes::CommandNode>("SequencerSetSave")->WaitUntilDone();
    }
    catch (std::exception e)
    {
        qDebug() << "[BackEnd::BackEnd] EXCEPTION: " << e.what();
    }
}

void BackEnd::switchToSequencerAcquisition()
{
    stopAcquisition();
    m_camera->enableSequencerConfiguration(false);
    m_camera->prepareAcquisition(AcquisitionType::CountedBurstTrigger, m_sequencerDuration);
    m_camera->enableSequencer(true);
    startAcquisition();
}

void BackEnd::switchToContinuousAcquisition()
{
    stopAcquisition();
    m_camera->enableSequencer(false);
    m_camera->prepareAcquisition(AcquisitionType::ContinousPwmTrigger, 25);
    m_camera->enableSequencerConfiguration(true);
    startAcquisition();
}

void BackEnd::softwareTrigger()
{
    if (m_acquisitionRunning)
        m_camera->softwareTrigger();
}

int BackEnd::sequencerDuration() const
{
    return m_sequencerDuration;
}

NodeListModel* BackEnd::nodeList() const
{
    return m_nodeList.get();
}

bool BackEnd::acquisitionRunning() const
{
    return m_acquisitionRunning;
}

QString BackEnd::Version() const
{
    return VERSION;
}

QString BackEnd::QtVersion() const
{
    return qVersion();
}
