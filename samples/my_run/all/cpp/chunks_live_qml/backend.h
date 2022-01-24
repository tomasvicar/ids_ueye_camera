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

#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QImage>
#include <QString>
#include <QThread>
#include <cstdint>

#include "acquisitionworker.h"
#include <peak/peak.hpp>

class BackEnd : public QObject
{
    Q_OBJECT

public:
    explicit BackEnd(QObject* parent = nullptr);
    ~BackEnd();

    Q_INVOKABLE bool start();

    Q_INVOKABLE bool openDevice();
    void closeDevice();

    Q_INVOKABLE QString Version() const;
    Q_INVOKABLE QString QtVersion() const;

private:
    std::shared_ptr<peak::core::Device> m_device;
    std::shared_ptr<peak::core::DataStream> m_dataStream;
    std::shared_ptr<peak::core::NodeMap> m_nodemapRemoteDevice;

    AcquisitionWorker* m_acquisitionWorker = nullptr;
    QThread m_acquisitionThread;

    QImage* m_image = new QImage;

signals:
    void imageReceived(QImage image, double chunkDataExposureTime_ms);
    void countersUpdated(unsigned int frameCounter, unsigned int errorCounter);
    void messageBoxTrigger(QString messageTitle, QString messageText);
};

#endif // BACKEND_H
