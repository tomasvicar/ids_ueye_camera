/*!
 * \file    acquisitionworker.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \brief   The AcquisitionWorker class is used in a worker thread to capture
 *          images from the device continuously and do an image conversion into
 *          a desired pixel format.
 *
 * \version 1.0.1
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

#include "acquisitionworker.h"

#include <QDebug>
#include <cmath>

#include <peak_ipl/peak_ipl.hpp>
#include <peak/converters/peak_buffer_converter_ipl.hpp>

AcquisitionWorker::AcquisitionWorker(QObject* parent) : QObject(parent)
{
    m_running = false;
    m_frameCounter = 0;
    m_errorCounter = 0;

    // 1 byte for each channel of RGBa
    m_bytesPerPixel = 4;
}

void AcquisitionWorker::start()
{
    try
    {
        // Lock critical features to prevent them from changing during acquisition
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("TLParamsLocked")->SetValue(1);

        // Start acquisition
        m_dataStream->StartAcquisition();
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->Execute();
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->WaitUntilDone();
    }
    catch (const std::exception& e)
    {
        qDebug() << "Exception: " << e.what();
        emit messageBoxTrigger("Exception", e.what());
    }

    m_running = true;

    while (m_running)
    {
        try
        {
            // Get buffer from device's datastream
            const auto buffer = m_dataStream->WaitForFinishedBuffer(5000);

            double chunkDataExposureTime_ms = -1;
            if (buffer->HasChunks())
            {
                m_nodemapRemoteDevice->UpdateChunkNodes(buffer);

                // Get the value of the exposure time chunk
                const auto chunkData = m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("ChunkExposureTime")->Value();
                chunkDataExposureTime_ms = round(chunkData) / 1000.0;
            }

            QImage qImage(m_imageWidth, m_imageHeight, QImage::Format_RGB32);

            // Create IDS peak IPL image for debayering and convert it to RGBa8 format
            const auto image = peak::BufferTo<peak::ipl::Image>(buffer).ConvertTo(
                peak::ipl::PixelFormatName::BGRa8, qImage.bits(), static_cast<size_t>(qImage.byteCount()));

            // Queue buffer so that it can be used again
            m_dataStream->QueueBuffer(buffer);

            // Emit signal that the image is ready to be displayed
            emit imageReceived(qImage, chunkDataExposureTime_ms);

            m_frameCounter++;
        }
        catch (const std::exception& e)
        {
            m_errorCounter++;

            qDebug() << "Exception: " << e.what();
            emit messageBoxTrigger("Exception", e.what());
        }

        // Send signal with current frame and error counter
        emit countersUpdated(m_frameCounter, m_errorCounter);
    }
}

void AcquisitionWorker::setDataStream(std::shared_ptr<peak::core::DataStream> dataStream)
{
    m_dataStream = dataStream;
}

void AcquisitionWorker::setNodemapRemoteDevice(std::shared_ptr<peak::core::NodeMap> nodeMap)
{
    m_nodemapRemoteDevice = nodeMap;

    m_imageWidth = static_cast<int>(m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("Width")->Value());
    m_imageHeight = static_cast<int>(m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("Height")->Value());
    m_size = static_cast<const size_t>(m_imageWidth * m_imageHeight * m_bytesPerPixel);
}

void AcquisitionWorker::stop()
{
    m_running = false;
}
