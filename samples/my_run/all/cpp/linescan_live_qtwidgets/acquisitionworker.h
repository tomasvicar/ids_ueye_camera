/*!
 * \file    acquisitionworker.h
 * \author  IDS Imaging Development Systems GmbH
 * \date    2021-01-15
 * \since   1.2.0
 *
 * \brief   The AcquisitionWorker class is used in a worker thread to capture
 *          images from the device continuously and do an image conversion into
 *          a desired pixel format.
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

#ifndef ACQUISITIONWORKER_H
#define ACQUISITIONWORKER_H

#include <peak/peak.hpp>

#include <QImage>
#include <QObject>
#include <QString>

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

    std::atomic_bool m_running;
    std::chrono::steady_clock::time_point m_timeStampLastFrame;

    std::string m_frameStartTriggerMode;
    std::string m_frameStartTriggerSource;
    std::string m_lineStartTriggerMode;
    std::string m_lineStartTriggerSource;

    unsigned int m_frameCounter = 0;
    unsigned int m_errorCounter = 0;

    int m_imageWidth = 0;
    int m_imageHeight = 0;
    int64_t m_bytesPerPixel = 0;
    size_t m_size = 0;

    void sendCountersUpdated();

signals:
    void imageReceived(QImage image);
    void countersUpdated(
        unsigned int frameCounter, unsigned int errorCounter, double frameRate_fps, double lineRate_lps);
    void messageBoxTrigger(QString messageTitle, QString messageText);
    void errorOccurred(QString errorText);
	void cameraDisconnected();

public slots:
    void triggerExecuted();
};

#endif // ACQUISITIONWORKER_H
