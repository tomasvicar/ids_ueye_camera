/*!
 * \file    imageitem.h
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

#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include <QImage>
#include <QPainter>
#include <QQuickItem>
#include <QQuickPaintedItem>


class ImageItem : public QQuickPaintedItem
{
    Q_OBJECT

public:
    ImageItem(QQuickItem* parent = nullptr);
    void paint(QPainter* painter);
    Q_INVOKABLE void setImage(QImage image);

private:
    QImage m_qimage;
    QPointF m_center;
};

#endif // IMAGEITEM_H
