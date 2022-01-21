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

#include "acquisitionworker.h"

#include <peak_ipl/peak_ipl.hpp>

#include <peak/converters/peak_buffer_converter_ipl.hpp>

#include <QDebug>

#include <cmath>
#include <cstring>


AcquisitionWorker::AcquisitionWorker(QObject* parent)
    : QObject(parent)
{
    m_running = false;
    m_frameCounter = 0;
    m_errorCounter = 0;
}


AcquisitionWorker::~AcquisitionWorker()
{}


void AcquisitionWorker::Start()
{
    try
    {
        // Start acquisition
        m_dataStream->StartAcquisition();
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->Execute();
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("AcquisitionStart")->WaitUntilDone();
    }
    catch (const std::exception& e)
    {
        qDebug() << "Exception: " << e.what();
    }

    m_running = true;

    while (m_running)
    {
        try
        {
            // Get buffer from device's datastream
            const auto buffer = m_dataStream->WaitForFinishedBuffer(5000);

            if ((AutoFeaturesManager::ExposureAutoMode::Off != m_autoFeaturesManager->GetExposureAutoMode())
                || (AutoFeaturesManager::GainAutoMode::Off != m_autoFeaturesManager->GetGainAutoMode())
                || (AutoFeaturesManager::BalanceWhiteAutoMode::Off != m_autoFeaturesManager->GetBalanceWhiteAutoMode()))
            {
                // Create image for the auto brightness calculator and apply ExposureAuto/GainAuto/BalanceWhiteAuto
                auto imageBrightness = peak::BufferTo<peak::ipl::Image>(buffer);
                (void)m_autoFeaturesManager->ProcessImage(imageBrightness);
            }

            if (m_autoFeaturesManager->IsExposureAutoFinished())
            {
                // Set mode to "off" which will reset the ExposureFinished flag
                m_autoFeaturesManager->SetExposureAutoMode(AutoFeaturesManager::ExposureAutoMode::Off);
                emit updateAutoControls();
            }

            if (m_autoFeaturesManager->IsGainAutoFinished())
            {
                // Set mode to "off" which will reset the GainFinished flag
                m_autoFeaturesManager->SetGainAutoMode(AutoFeaturesManager::GainAutoMode::Off);
                emit updateAutoControls();
            }

            if (m_autoFeaturesManager->IsBalanceWhiteAutoFinished())
            {
                // Set mode to "off" which will reset the BalanceWhiteFinished flag
                m_autoFeaturesManager->SetBalanceWhiteAutoMode(AutoFeaturesManager::BalanceWhiteAutoMode::Off);
                emit updateAutoControls();
            }

            QImage image(m_imageWidth, m_imageHeight, QImage::Format_RGB888);

            // Create IDS peak IPL image for debayering and convert it to RGB8 format
            const auto iplImage = peak::BufferTo<peak::ipl::Image>(buffer).ConvertTo(
                peak::ipl::PixelFormatName::RGB8, image.bits(), static_cast<size_t>(image.byteCount()));

            // Queue buffer so that it can be used again
            m_dataStream->QueueBuffer(buffer);

            // Emit signal that the image is ready to be displayed
            emit imageReceived(image);

            m_frameCounter++;
        }
        catch (const std::exception& e)
        {
            m_errorCounter++;

            qDebug() << "Exception: " << e.what();
        }

        // Send signal with current frame and error counter
        emit counterUpdated(m_frameCounter, m_errorCounter);
    }
}

void AcquisitionWorker::Stop()
{
    m_running = false;
}


void AcquisitionWorker::SetDataStream(std::shared_ptr<peak::core::DataStream> dataStream)
{
    m_dataStream = dataStream;
    m_nodemapRemoteDevice = m_dataStream->ParentDevice()->RemoteDevice()->NodeMaps().at(0);

    m_imageWidth = static_cast<int>(
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("Width")->Value());

    m_imageHeight = static_cast<int>(
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("Height")->Value());
}


void AcquisitionWorker::SetAutoFeaturesManager(AutoFeaturesManager* autoFeaturesManager)
{
    m_autoFeaturesManager = autoFeaturesManager;
}
