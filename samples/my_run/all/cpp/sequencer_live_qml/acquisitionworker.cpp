/*!
 * \file    acquisitionworker.cpp
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

#include "acquisitionworker.h"

#include <peak/common/peak_common_structs.hpp>
#include <peak/data_stream/peak_data_stream.hpp>

#include <QDebug>
#include <QImage>
#include <QThread>

AcquisitionWorker::AcquisitionWorker()
{
    m_frameCounter = 0;
    m_errorCounter = 0;
}

AcquisitionWorker::~AcquisitionWorker()
{}

AcquisitionWorker::WorkerState AcquisitionWorker::currentState()
{
    return m_state;
}

void AcquisitionWorker::Start()
{
    m_state = RUNNING;

    while (m_state == RUNNING)
    {
        try
        {
            // get buffer from device's datastream
            auto buffer = m_dataStream->WaitForFinishedBuffer(peak::core::Timeout::INFINITE_TIMEOUT);

            // if a buffer was received, emit signal and increase the frame counter
            if (buffer)
            {
                emit bufferReceived(buffer);
                ++m_frameCounter;
                emit counterUpdated(m_frameCounter, m_errorCounter);
            }
        }
        catch (const std::exception& e)
        {
            // if there was an exception during running image acquisition, increase the error counter
            if (m_state == RUNNING)
            {
                qDebug() << "[AcquisitionWorker] EXCEPTION: " << e.what();
                ++m_errorCounter;
                emit counterUpdated(m_frameCounter, m_errorCounter);
            }
        }

        // while the worker is paused, wait...
        while (m_state == PAUSED)
        {
            QThread::msleep(50);
        }
    }
    qDebug() << "[AcquisitionWorker] acquisition worker stopped";
}

void AcquisitionWorker::SetDataStream(std::shared_ptr<peak::core::DataStream> dataStream)
{
    m_dataStream = dataStream;
}

void AcquisitionWorker::Stop()
{
    m_state = IDLE;
}

void AcquisitionWorker::Pause()
{
    m_state = PAUSED;
}

void AcquisitionWorker::Resume()
{
    m_state = RUNNING;
}
