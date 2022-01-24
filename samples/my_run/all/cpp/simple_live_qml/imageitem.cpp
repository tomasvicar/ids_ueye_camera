/*!
 * \file    imageitem.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2019-05-01
 * \since   1.0.0
 *
 * \version 1.0.0
 *
 * Copyright (C) 2019 - 2021, IDS Imaging Development Systems GmbH.
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

#include "imageitem.h"

#include <QtConcurrent/QtConcurrent>
#include <QDebug>


ImageItem::ImageItem(QQuickItem* parent)
    : QQuickPaintedItem(parent)
{
    // initial transparent image
    m_qimage = QImage(100, 100, QImage::Format_ARGB32);
    m_qimage.fill(Qt::GlobalColor::transparent);
    QQuickPaintedItem::setRenderTarget(QQuickPaintedItem::FramebufferObject);
}

void ImageItem::paint(QPainter* painter)
{
    painter->drawImage(m_center, m_qimage);
}

void ImageItem::setImage(QImage image)
{
    auto boundingRectangle = boundingRect();
    m_qimage = image.scaled(
        static_cast<int>(boundingRectangle.width()), static_cast<int>(boundingRectangle.height()), Qt::KeepAspectRatio);
    m_center = boundingRectangle.center() - m_qimage.rect().center();
    update();
}
