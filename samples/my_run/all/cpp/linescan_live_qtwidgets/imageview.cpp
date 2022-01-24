/*!
 * \file    imagesview.cpp
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

#include "imageview.h"
#include "imagescene.h"

#include <QDebug>
#include <QGraphicsView>
#include <QString>

#include <cmath>

ImageView::ImageView(QWidget* parent, int64_t imageWidth, int64_t imageHeight)
    : QGraphicsView(parent)
{
    m_imageWidth = std::max<int64_t>(imageWidth, 1);
    m_imageHeight = std::max<int64_t>(imageHeight, 1);

    m_image = QImage(static_cast<int>(m_imageWidth), static_cast<int>(m_imageHeight), QImage::Format_RGB32);
    m_imageScene = new ImageScene(this);

    // Connect the signal from the image scene when an exception was thrown and a message should be printed
    // with the messagebox show signal in the ImageView class
    connect(m_imageScene, &ImageScene::messageBoxTrigger, this, &ImageView::messageBoxTrigger);

    setScene(m_imageScene);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

QImage ImageView::getImage() const
{
    return m_image;
}

int64_t ImageView::getImageWidth() const
{
    return m_imageWidth;
}

int64_t ImageView::getImageHeight() const
{
    return m_imageHeight;
}

void ImageView::updateImage(QImage image)
{
    m_image = image;
    m_imageScene->update();
}
