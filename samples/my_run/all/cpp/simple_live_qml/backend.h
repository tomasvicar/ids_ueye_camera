/*!
 * \file    backend.h
 * \author  IDS Imaging Development Systems GmbH
 * \date    2019-05-01
 * \since   1.0.0
 *
 * \version 1.0.0
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

#ifndef BACKEND_H
#define BACKEND_H

#include "acquisitionworker.h"

#include <peak/peak.hpp>

#include <QImage>
#include <QObject>
#include <QString>
#include <QThread>


class BackEnd : public QObject
{
    Q_OBJECT

public:
    explicit BackEnd(QObject* parent = nullptr);
    ~BackEnd();

    Q_INVOKABLE void OpenDevice();
    void CloseDevice();

    Q_INVOKABLE QString Version() const;
    Q_INVOKABLE QString QtVersion() const;

signals:
    void acquisitionStarted();
    void imageChanged(QImage image);
    void counterChanged(const unsigned int frameCounter, const unsigned int errorCounter);
    void messageBoxTrigger(QString messageTitle, QString messageText);

private:
    std::shared_ptr<peak::core::Device> m_device;
    std::shared_ptr<peak::core::DataStream> m_dataStream;
	std::shared_ptr<peak::core::NodeMap> m_nodemapRemoteDevice;

    AcquisitionWorker* m_acquisitionWorker;
    QThread m_acquisitionThread;
};

#endif // BACKEND_H
