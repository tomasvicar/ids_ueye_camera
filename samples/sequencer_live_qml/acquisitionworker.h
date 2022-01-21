/*!
 * \file    acquisitionworker.h
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \brief   The AcquisitionWorker class is used in a worker thread to capture
 *          images from the device continuously.
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

#ifndef ACQUISITIONWORKER_H
#define ACQUISITIONWORKER_H

#include <peak/data_stream/peak_data_stream.hpp>

#include <QObject>

class AcquisitionWorker : public QObject
{
    Q_OBJECT
public:
    AcquisitionWorker();
    ~AcquisitionWorker();
    void Start();
    void Stop();
    void Pause();
    void Resume();
    void SetDataStream(std::shared_ptr<peak::core::DataStream>);
    enum WorkerState
    {
        IDLE,
        RUNNING,
        PAUSED
    };

    WorkerState currentState();

signals:
    void bufferReceived(std::shared_ptr<peak::core::Buffer> buffer);
    void counterUpdated(const unsigned int, const unsigned int);
    void finished();

private:
    std::shared_ptr<peak::core::DataStream> m_dataStream;
    unsigned int m_frameCounter;
    unsigned int m_errorCounter;
    WorkerState m_state = IDLE;
};

Q_DECLARE_METATYPE(std::shared_ptr<peak::core::Buffer>)

#endif // ACQUISITIONWORKER_H
