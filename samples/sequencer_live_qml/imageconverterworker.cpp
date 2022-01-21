/*!
 * \file    imageconverterworker.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \brief   The ImageConverter class is used in a worker thread to convert
 *          buffers received from the datastream to images that can be displayed.
 *
 * \version 1.0.1
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

#include "imageconverterworker.h"

#include <peak/converters/peak_buffer_converter_ipl.hpp>
#include <peak/device/peak_device.hpp>
#include <peak/node_map/peak_node_map.hpp>

#include <QDebug>

ImageConverterWorker::ImageConverterWorker()
{}

void ImageConverterWorker::setDataStream(const std::shared_ptr<peak::core::DataStream>& dataStream)
{
    m_dataStream = dataStream;
}

void ImageConverterWorker::setImageCount(const unsigned int imageCount)
{
    m_imageCount = imageCount;
}

void ImageConverterWorker::convert(std::shared_ptr<peak::core::Buffer> buffer)
{
    if (!buffer)
        return;

    try
    {
        if (buffer->IsIncomplete())
        {
            qDebug() << "[ImageConverterWorker::convert] Buffer incomplete";
        }

        int chunkInfo = -1;
        unsigned long long timestamp_us = 0;

        // if the buffer has chunks retrieve information about the current sequencer set from it
        if (buffer->HasChunks())
        {
            // update the chunk nodes of the node map of the remote device
            auto m_nodeMapRemoteDevice = m_dataStream->ParentDevice()->RemoteDevice()->NodeMaps().at(0);
            m_nodeMapRemoteDevice->UpdateChunkNodes(buffer);

            // read the current sequencer set from chunk
            chunkInfo = m_nodeMapRemoteDevice
                            ->FindNode<peak::core::nodes::IntegerNode>("ChunkSequencerSetActive")
                            ->Value();

            try
            {
                // for IDS cameras, try using the ChunkTimestamp value
                // for other cameras, don't use that, because the value might not be ns
                const auto vendorName = m_nodeMapRemoteDevice
                                            ->FindNode<peak::core::nodes::StringNode>("DeviceVendorName")
                                            ->Value();
                if (vendorName.find("IDS") != std::string::npos)
                {
                    timestamp_us = m_nodeMapRemoteDevice
                                       ->FindNode<peak::core::nodes::IntegerNode>("ChunkTimestamp")
                                       ->Value()
                        / 1000;
                }
            }
            catch (const peak::core::Exception&)
            { /* ignore, use Timestamp_ns() below */
            }
        }

        if (timestamp_us == 0)
        {
            // read the timestamp of the current image from the buffer info
            try
            {
                timestamp_us = buffer->Timestamp_ns() / 1000;
            }
            catch (const peak::core::InternalErrorException&)
            { /* ignore, keep timestamp_us at 0 */
            }
        }

        // create image for debayering and convert it to RGB8 format
        auto image = peak::BufferTo<peak::ipl::Image>(buffer).ConvertTo(peak::ipl::PixelFormatName::RGB8);

        // queue buffer so that it can be used again
        m_dataStream->QueueBuffer(buffer);

        // convert IDS peak IPL image to QImage and store it in vector
        auto index = m_converterCounter++ % m_imageCount;
        auto qImage = QImage(static_cast<int>(image.Width()), static_cast<int>(image.Height()), QImage::Format_RGB888);
        memcpy(qImage.bits(), image.Data(), image.ByteCount());

        // emit signal that the image is ready to be displayed
        if (chunkInfo == -1)
        {
            emit imageReceived(
                qImage, index % (m_imageCount / 2), timestamp_us, timestamp_us - m_timestamp_previous_us);
        }
        else
        {
            emit imageReceived(qImage, chunkInfo, timestamp_us, timestamp_us - m_timestamp_previous_us);
        }

        m_timestamp_previous_us = timestamp_us;
    }
    catch (const std::exception& e)
    {
        qDebug() << "[ImageConverterWorker::convert] EXCEPTION: " << e.what();

        // try to queue buffer
        try
        {
            if (!buffer->IsQueued())
            {
                m_dataStream->QueueBuffer(buffer);
            }
        }
        catch (const std::exception& e)
        {
            qDebug() << "[ImageConverterWorker::convert] EXCEPTION: " << e.what();
        }
        return;
    }
}

void ImageConverterWorker::resetCounter()
{
    m_converterCounter = 0;
}
