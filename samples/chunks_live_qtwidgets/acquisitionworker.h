/*!
 * \file    acquisitionworker.h
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \brief   The AcquisitionWorker class is used in a worker thread to capture
 *          images from the device continuously and do an image conversion into
 *          a desired pixel format.
 *
 * \version 1.0.0
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

#ifndef ACQUISITIONWORKER_H
#define ACQUISITIONWORKER_H

#include <QObject>
#include <QString>
#include <QImage>

#include <peak/peak.hpp>

class AcquisitionWorker : public QObject
{
    Q_OBJECT

public:
    AcquisitionWorker(QObject* parent = nullptr);

    void start();
    void stop();
    void setDataStream(std::shared_ptr<peak::core::DataStream> dataStream);
    void setNodemapRemoteDevice(std::shared_ptr<peak::core::NodeMap> nodeMap);

    int getImageWidth() const;
    int getImageHeight() const;

private:
    std::shared_ptr<peak::core::DataStream> m_dataStream;
    std::shared_ptr<peak::core::NodeMap> m_nodemapRemoteDevice;

    bool m_running = false;

    unsigned int m_frameCounter = 0;
    unsigned int m_errorCounter = 0;

    int m_imageWidth = 0;
    int m_imageHeight = 0;
    int64_t m_bytesPerPixel = 0;
    size_t m_size = 0;

signals:
    void imageReceived(QImage image, double chunkDataExposureTime_ms);
    void countersUpdated(unsigned int frameCounter, unsigned int errorCounter);
    void messageBoxTrigger(QString messageTitle, QString messageText);
};

#endif // ACQUISITIONWORKER_H
