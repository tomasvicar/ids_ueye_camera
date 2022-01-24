/*!
 * \file    imagescene.cpp
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

#include "imagescene.h"
#include "imageview.h"

#include <QGraphicsTextItem>

#include <cmath>
#include <stdexcept>

ImageScene::ImageScene(ImageView* parent)
    : QGraphicsScene(parent)
{
    m_parent = parent;
}

void ImageScene::drawBackground(QPainter* painter, const QRectF&)
{
    setSceneRect(0, 0, m_parent->width(), m_parent->height());

    // Display size
    const double displayWidth = static_cast<double>(m_parent->width());
    const double displayHeight = static_cast<double>(m_parent->height());

    // Image size
    double imageWidth = static_cast<double>(m_parent->getImageWidth());
    double imageHeight = static_cast<double>(m_parent->getImageHeight());

    try
    {
        double ratio1 = 0.0;
        double ratio2 = 0.0;

        if (imageWidth == 0.0 || imageHeight == 0.0)
        {
            throw std::overflow_error("Attempted to divide with zero");
        }
        else
        {
            // Calculate aspect ratio of the display
            ratio1 = displayWidth / displayHeight;

            // Calculate aspect ratio of the image
            ratio2 = imageWidth / imageHeight;
        }

        if (ratio1 >= ratio2)
        {
            // The height with must fit to the display height. So h remains and w must be scaled down
            imageWidth = displayHeight * ratio2;
            imageHeight = displayHeight;
        }
        else
        {
            if (ratio2 == 0.0)
            {
                throw std::overflow_error("Attempted to divide with zero");
            }
            else
            {
                // The image with must fit to the display width. So w remains and h must be scaled down
                imageWidth = displayWidth;
                imageHeight = displayWidth / ratio2;
            }
        }

        const double imagePosX = (displayWidth - imageWidth) / 2.0;
        const double imagePosY = (displayHeight - imageHeight) / 2.0;

        QFont font;
        font.setPixelSize(static_cast<int>(std::sqrt(imageWidth * imageHeight) / 25));

        QRectF rect(imagePosX, imagePosY, imageWidth, imageHeight);
        painter->drawImage(rect, m_parent->getImage());
    }
    catch (const std::overflow_error& e)
    {
        emit messageBoxTrigger("Exception", e.what());
    }
}
