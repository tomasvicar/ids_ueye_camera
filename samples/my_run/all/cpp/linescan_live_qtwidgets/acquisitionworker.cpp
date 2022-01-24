/*!
 * \file    acquisitionworker.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2021-01-15
 * \since   1.2.0
 *
 * \brief   The AcquisitionWorker class is used in a worker thread to capture
 *          images from the device continuously and do an image conversion into
 *          a desired pixel format.
 *
 * \version 1.0.1
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

#include "acquisitionworker.h"

#include <peak_ipl/peak_ipl.hpp>
#include <peak/converters/peak_buffer_converter_ipl.hpp>

#include <QDebug>

#include <chrono>
#include <cmath>

AcquisitionWorker::AcquisitionWorker(QObject* parent)
    : QObject(parent)
{
    m_running = false;
    m_timeStampLastFrame = std::chrono::steady_clock::now();
    m_frameCounter = 0;
    m_errorCounter = 0;

    // 1 byte for each channel of RGBa
    m_bytesPerPixel = 4;
}

void AcquisitionWorker::start()
{
    try
    {
        // Start acquisition
        m_dataStream->StartAcquisition();
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->Execute();
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->WaitUntilDone();

        m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
            ->SetCurrentEntry("FrameStart");
        m_frameStartTriggerMode = m_nodemapRemoteDevice
                                      ->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                                      ->CurrentEntry()
                                      ->StringValue();
        m_frameStartTriggerSource = m_nodemapRemoteDevice
                                        ->FindNode<peak::core::nodes::EnumerationNode>("TriggerSource")
                                        ->CurrentEntry()
                                        ->StringValue();

        m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
            ->SetCurrentEntry("LineStart");
        m_lineStartTriggerMode = m_nodemapRemoteDevice
                                     ->FindNode<peak::core::nodes::EnumerationNode>("TriggerMode")
                                     ->CurrentEntry()
                                     ->StringValue();
        m_lineStartTriggerSource = m_nodemapRemoteDevice
                                       ->FindNode<peak::core::nodes::EnumerationNode>("TriggerSource")
                                       ->CurrentEntry()
                                       ->StringValue();
    }
    catch (const std::exception& e)
    {
        qDebug() << "Exception: " << e.what();
        emit messageBoxTrigger("Exception", e.what());
    }

    // If software trigger not active
    if (!(m_frameStartTriggerSource == "Software" && m_frameStartTriggerMode == "On"))
    {
        m_running = true;

        while (m_running)
        {
            try
            {
                auto bufferDuration = 0;
                if (m_frameStartTriggerMode == "Off" && m_lineStartTriggerMode == "Off")
                {
                    bufferDuration = 5000
                        + static_cast<int>(m_imageWidth
                              / std::round(m_nodemapRemoteDevice
                                               ->FindNode<peak::core::nodes::FloatNode>("AcquisitionLineRate")
                                               ->Value()))
                            * 1000;
                }
                else if (m_lineStartTriggerMode == "On" && m_lineStartTriggerSource == "PWM0")
                {
                    bufferDuration = 5000
                        + static_cast<int>(m_imageWidth
                              / std::round(
                                  m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("PWMFrequency")
                                      ->Value()))
                            * 1000;
                }
                else
                {
                    bufferDuration = 15000;
                }

                // Get buffer from device's datastream
                const auto buffer = m_dataStream->WaitForFinishedBuffer(bufferDuration);

                QImage qImage(m_imageWidth, m_imageHeight, QImage::Format_RGB32);

                // Create IDS peak IPL image for debayering and convert it to RGBa8 format
                const auto image = peak::BufferTo<peak::ipl::Image>(buffer).ConvertTo(
                    peak::ipl::PixelFormatName::BGRa8, qImage.bits(), static_cast<size_t>(qImage.byteCount()));

                // Queue buffer so that it can be used again
                m_dataStream->QueueBuffer(buffer);

                // Emit signal that the image is ready to be displayed
                emit imageReceived(qImage);

                m_frameCounter++;
            }
			catch (const peak::core::InternalErrorException& e)
			{
				qDebug() << "Exception: " << e.what();
				emit cameraDisconnected();
				m_running = false;
				break;
			}
            catch (const std::exception& e)
            {
                if (m_running)
                {
                    m_errorCounter++;

                    qDebug() << "Exception: " << e.what();
                    emit errorOccurred(e.what());
                }
            }

            sendCountersUpdated();
        }
    }
    else
    {
        try
        {
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                ->SetCurrentEntry("FrameStart");
        }
        catch (const std::exception& e)
        {
            qDebug() << "Exception: " << e.what();
            emit messageBoxTrigger("Exception", e.what());
        }
    }
}

void AcquisitionWorker::triggerExecuted()
{
    try
    {
        auto bufferDuration = 0;
        if (m_lineStartTriggerMode == "On" && m_lineStartTriggerSource == "PWM0")
        {
            bufferDuration = 5000
                + static_cast<int>(std::round(m_imageWidth
                    / m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("PWMFrequency")->Value()));
        }
        else
        {
            bufferDuration = 15000;
        }

        // Get buffer from device's datastream
        const auto buffer = m_dataStream->WaitForFinishedBuffer(bufferDuration);

        QImage qImage(m_imageWidth, m_imageHeight, QImage::Format_RGB32);

        // Create IDS peak IPL image for debayering and convert it to RGBa8 format
        const auto image = peak::BufferTo<peak::ipl::Image>(buffer).ConvertTo(
            peak::ipl::PixelFormatName::BGRa8, qImage.bits(), static_cast<size_t>(qImage.byteCount()));

        // Queue buffer so that it can be used again
        m_dataStream->QueueBuffer(buffer);

        // Emit signal that the image is ready to be displayed
        emit imageReceived(qImage);

        m_frameCounter++;
    }
    catch (const std::exception& e)
    {
        m_errorCounter++;

        qDebug() << "Exception: " << e.what();
        emit errorOccurred(e.what());
    }

    sendCountersUpdated();
}

void AcquisitionWorker::sendCountersUpdated()
{
    auto frameRate_fps = -1.0;
    auto lineRate_lps = -1.0;

    try
    {
        if (m_frameStartTriggerMode == "Off")
        {
            frameRate_fps =
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("AcquisitionFrameRate")->Value();
        }

        if (m_lineStartTriggerMode == "Off")
        {
            lineRate_lps = m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("AcquisitionLineRate")
                               ->Value();
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "Exception: " << e.what();
        emit errorOccurred(e.what());
    }

    // Send signal with current frame and error counter
    emit countersUpdated(m_frameCounter, m_errorCounter, frameRate_fps, lineRate_lps);
}

void AcquisitionWorker::setDataStream(std::shared_ptr<peak::core::DataStream> dataStream)
{
    m_dataStream = dataStream;
}

void AcquisitionWorker::setNodemapRemoteDevice(std::shared_ptr<peak::core::NodeMap> nodeMap)
{
    m_nodemapRemoteDevice = nodeMap;

    try
    {
        m_imageWidth = static_cast<int>(
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("Width")->Value());
        m_imageHeight = static_cast<int>(
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("Height")->Value());
    }
    catch (const std::exception& e)
    {
        qDebug() << "Exception: " << e.what();
        emit messageBoxTrigger("Exception", e.what());
    }
    m_size = static_cast<const size_t>(m_imageWidth * m_imageHeight * m_bytesPerPixel);
}

void AcquisitionWorker::stop()
{
    m_running = false;
    if (m_dataStream)
    {
        m_dataStream->KillWait();
    }
}

int AcquisitionWorker::getImageWidth() const
{
    return m_imageWidth;
}

int AcquisitionWorker::getImageHeight() const
{
    return m_imageHeight;
}
