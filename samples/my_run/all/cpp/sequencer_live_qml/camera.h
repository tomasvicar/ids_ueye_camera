/*!
 * \file    camera.h
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

#ifndef CAMERA_H
#define CAMERA_H

#include "acquisitionworker.h"
#include "imageconverterworker.h"
#include "nodelistmodel.h"

#include <peak/data_stream/peak_data_stream.hpp>
#include <peak/device/peak_device.hpp>
#include <peak/device/peak_device_descriptor.hpp>
#include <peak/node_map/peak_node_map.hpp>

#include <QImage>
#include <QObject>
#include <QThread>

#include <string>

enum AcquisitionType
{
    Freerun = 0,
    SoftwareTrigger,
    ContinousPwmTrigger,
    CountedBurstTrigger
};

class Camera : public QObject
{
    Q_OBJECT
public:
    Camera(std::shared_ptr<peak::core::DeviceDescriptor> deviceDescriptor);
    void close();

    std::string key();
    std::shared_ptr<NodeListModel> nodeList();
    std::shared_ptr<peak::core::NodeMap> nodeMapRemoteDevice() const;

    void setBufferCountMinSequencer(int bufferCountMinSequencer);

    void loadUserSet(std::string userSet = "Default");
    void prepareAcquisition(AcquisitionType type = AcquisitionType::Freerun, int param = 0);
    bool startAcquisition();
    bool stopAcquisition();
    void enableSequencerConfiguration(bool mode);
    void enableSequencer(bool mode);

signals:
    void imageReceived(
        QImage image, unsigned int iterator, unsigned long long timestamp, unsigned long long timestampDelta);
    void counterReceived(const unsigned int, const unsigned int);
    void nodeListChanged();

public slots:
    void onCounterReceived(const unsigned int, const unsigned int);
    void softwareTrigger();
    void updateNodeList();

private:
    std::shared_ptr<peak::core::Device> m_device;
    std::shared_ptr<peak::core::DataStream> m_dataStream;
    std::shared_ptr<peak::core::NodeMap> m_nodeMapRemoteDevice;
    std::shared_ptr<NodeListModel> m_nodeList;

    AcquisitionWorker* m_acquisitionWorker;
    QThread* m_acquisitionThread;
    ImageConverterWorker* m_converterWorker;
    QThread* m_converterThread;

    int m_bufferCountMinSequencer = 8;
    bool m_acquisitionStarted;
    std::shared_ptr<peak::core::nodes::EnumerationNode> m_triggerSelectorNode;
    std::string m_triggerSelectorEntry;
    std::shared_ptr<peak::core::nodes::CommandNode> m_triggerNode;
};

#endif // CAMERA_H
