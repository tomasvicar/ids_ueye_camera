/*!
 * \file    backend.h
 * \author  IDS Imaging Development Systems GmbH
 * \date    2021-01-15
 * \since   1.2.0
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

#ifndef BACKEND_H
#define BACKEND_H

#include "acquisitionworker.h"
#include "configdialog.h"

#include <peak/peak.hpp>

#include <QImage>
#include <QObject>
#include <QString>
#include <QThread>

#include <cstdint>

class BackEnd : public QObject
{
    Q_OBJECT

public:
    explicit BackEnd(QObject* parent = nullptr);
    ~BackEnd();

    bool start();
    bool openDevice();
    void closeDevice();

    int getImageWidth() const;
    int getImageHeight() const;
    std::shared_ptr<peak::core::NodeMap> getNodemapRemoteDevice();
    std::pair<bool, std::pair<QString, QString>> getFrameStartTrigger();
    std::pair<bool, std::pair<QString, QString>> getLineStartTrigger();

private:
    std::shared_ptr<peak::core::Device> m_device;
    std::shared_ptr<peak::core::DataStream> m_dataStream;
    std::shared_ptr<peak::core::NodeMap> m_nodemapRemoteDevice;

    ConfigDialog* m_configDialog = nullptr;
    AcquisitionWorker* m_acquisitionWorker = nullptr;
    QThread m_acquisitionThread;

    bool m_submitted = false;

    std::pair<bool, std::pair<QString, QString>> m_frameStartTrigger;
    std::pair<bool, std::pair<QString, QString>> m_lineStartTrigger;

signals:
    void imageReceived(QImage image);
    void countersUpdated(unsigned int frameCounter, unsigned int errorCounter, double frameRate_ms, double lineRate_ms);
    void messageBoxTrigger(QString messageTitle, QString messageText);
	void cameraDisconnected();
    void errorOccurred(QString errorText);
    void started(bool backendStarted);
    void triggerExecuted();

private slots:
    void handleConfigSubmitted(int offsetY, int imageHeight,
        std::pair<bool, std::pair<QString, QString>> frameStartTrigger,
        std::pair<bool, std::pair<QString, QString>> lineStartTrigger, int acquisitionLineRate, int PWMFrequency);
    void handleConfigDialogClosed();
};

#endif // BACKEND_H
