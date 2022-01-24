/*!
 * \file    imageitem.h
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

#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include <QImage>
#include <QPainter>
#include <QQuickItem>
#include <QQuickPaintedItem>

class ImageItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QSize sourceSize READ sourceSize NOTIFY sourceSizeChanged)
    Q_PROPERTY(QSize boundsSize READ boundsSize WRITE setBoundsSize NOTIFY boundsSizeChanged)
    Q_PROPERTY(QImage sourceImage READ sourceImage NOTIFY sourceImageChanged)

public:
    ImageItem(QQuickItem* parent = nullptr);
    void paint(QPainter* painter);

    QSize sourceSize();
    QSize boundsSize();
    QImage sourceImage();

    Q_INVOKABLE void setImage(QImage image);
    Q_INVOKABLE void saveSourceImage(QString url);
    Q_INVOKABLE void fitToBounds();

signals:
    void sourceSizeChanged();
    void boundsSizeChanged();
    void sourceImageChanged();
    void sizeChanged(QSizeF size);

public slots:
    void setBoundsSize(QSize newSize);

private:
    QImage m_qimage;
    QImage m_sourceImage;
    QSize m_sourceSize;
    QSize m_boundsSize;
    qreal m_currentScale = 1;
};

#endif // IMAGEITEM_H
