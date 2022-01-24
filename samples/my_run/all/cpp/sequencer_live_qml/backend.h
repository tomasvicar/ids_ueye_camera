/*!
 * \file    backend.h
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
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

#ifndef BACKEND_H
#define BACKEND_H

#include "acquisitionworker.h"
#include "camera.h"
#include "imageconverterworker.h"
#include "nodelistmodel.h"

#include <peak/data_stream/peak_data_stream.hpp>
#include <peak/device/peak_device.hpp>
#include <peak/device/peak_device_descriptor.hpp>
#include <peak/node_map/peak_node_map.hpp>

#include <QObject>
#include <string>
#include <vector>

class BackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(NodeListModel* nodeList READ nodeList NOTIFY nodeListChanged)
    Q_PROPERTY(bool acquisitionRunning READ acquisitionRunning NOTIFY acquisitionRunningChanged)
    Q_PROPERTY(int sequencerDuration READ sequencerDuration NOTIFY sequencerDurationChanged)

public:
    explicit BackEnd(QObject* parent = nullptr);
    ~BackEnd();

    // Getter functions for Q_PROPERTY
    NodeListModel* nodeList() const;
    bool acquisitionRunning() const;
    void updateCameraList();
    int sequencerDuration() const;

    // Handling Cameras
    Q_INVOKABLE void openCamera(int index = -1);
    void closeCamera();

    // Start and stop image acquisition
    Q_INVOKABLE void startAcquisition();
    Q_INVOKABLE void stopAcquisition();

    // Some special parameter functions
    Q_INVOKABLE void applyPixelFormat(int index);
    Q_INVOKABLE void applyRoi(QRect roi);
    Q_INVOKABLE void resetRoi();

    // Sequencer related functions
    Q_INVOKABLE void prepareSequencer(int sequencerDuration);
    Q_INVOKABLE void loadSequencerSet(int index);
    Q_INVOKABLE void saveSequencerSet(int index);


    Q_INVOKABLE void init();

    Q_INVOKABLE void cameraSettingsStartup();

    Q_INVOKABLE QString Version() const;
    Q_INVOKABLE QString QtVersion() const;

signals:
    // NOTIFY signals for Q_PROPERTY
    void nodeListChanged();
    void sequenceListChanged();
    void acquisitionRunningChanged();
    void sequencerDurationChanged();

    // Other signals
    void imageReceived(
        QImage image, unsigned int iterator, unsigned long long timestamp, unsigned long long timestampDelta);
    void cameraOpened();
    void cameraAboutToClose();
    void cameraClosed();
    void closing();
    void error(QString message);
    void startupFinished();

public slots:
    void switchToSequencerAcquisition();
    void switchToContinuousAcquisition();
    void softwareTrigger();

private:
    // Private variables for Q_PROPERTY
    std::shared_ptr<NodeListModel> m_nodeList;
    bool m_acquisitionRunning = false;
    int m_sequencerDuration = 4;

    // the IDS peak API objects
    std::shared_ptr<peak::core::Device> m_device = nullptr;
    std::shared_ptr<peak::core::NodeMap> m_nodeMap = nullptr;
    std::shared_ptr<peak::core::DataStream> m_dataStream = nullptr;
    std::vector<std::shared_ptr<peak::core::DeviceDescriptor>> m_devices;

    // some other privates
    std::shared_ptr<Camera> m_camera = nullptr;
    std::string m_openedCamera = "";
};

#endif // BACKEND_H
