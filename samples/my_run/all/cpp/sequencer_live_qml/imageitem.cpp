/*!
 * \file    imageitem.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \brief   The ImageItem class is used as container to display a camera image
 *          in QML
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

#include "imageitem.h"

#include <QtConcurrent/QtConcurrent>
#include <QDebug>
#include <QImageWriter>

#include <iostream>

ImageItem::ImageItem(QQuickItem* parent)
    : QQuickPaintedItem(parent)
{
    // initial transparent image
    m_qimage = QImage(100, 100, QImage::Format_ARGB32);
    m_qimage.fill(Qt::GlobalColor::transparent);
    QQuickPaintedItem::setRenderTarget(QQuickPaintedItem::Image);
    m_sourceSize = QSize(100, 100);
    m_sourceImage = m_qimage;
}

void ImageItem::paint(QPainter* painter)
{
    try
    {
        // scale the sourceImage for display
        m_qimage = m_sourceImage.scaled(static_cast<int>(width()), static_cast<int>(height()), Qt::KeepAspectRatio);
        painter->drawImage(QPoint(0, 0), m_qimage);
    }
    catch (const std::exception& e)
    {
        qDebug() << "[ImageItem::paint] EXCEPTION: " << e.what();
        return;
    }
}

QSize ImageItem::sourceSize()
{
    return m_sourceSize;
}

QSize ImageItem::boundsSize()
{
    return m_boundsSize;
}

QImage ImageItem::sourceImage()
{
    return m_sourceImage;
}

Q_INVOKABLE void ImageItem::setImage(QImage image)
{
    // check that image is not empty
    if (image.isNull())
    {
        return;
    }

    m_sourceImage = image;

    // if the size of the image changed, set new width and height
    if (m_sourceImage.size() != m_sourceSize)
    {
        m_sourceSize = m_sourceImage.size();
        qDebug() << "Source size changed to " << m_sourceSize.width() << " x " << m_sourceSize.height();

        setWidth(m_sourceSize.width() * m_currentScale);
        setHeight(m_sourceSize.height() * m_currentScale);
        emit sourceSizeChanged();

        fitToBounds();
    }

    // update the display
    update();
}

void ImageItem::saveSourceImage(QString url)
{
    // save the image with the original resolution
    m_sourceImage.save(url);
}

void ImageItem::fitToBounds()
{
    // make the display fit to the current object bounds
    qreal s = std::min(static_cast<qreal>(m_boundsSize.width()) / m_sourceSize.width(),
        static_cast<qreal>(m_boundsSize.height()) / m_sourceSize.height());
    setWidth(m_sourceSize.width() * s);
    setHeight(m_sourceSize.height() * s);
    m_currentScale = s;
}

void ImageItem::setBoundsSize(QSize newSize)
{
    // set the size of the object bounds
    m_boundsSize = newSize;
    emit boundsSizeChanged();
}
