/*!
 * \file    imagesview.h
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

#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <QGraphicsView>
#include <QImage>
#include <QObject>

#include <cstdint>

class ImageScene;

class ImageView : public QGraphicsView
{
    Q_OBJECT

public:
    ImageView(QWidget* parent, int64_t imageWidth, int64_t imageHeight);

    QImage getImage() const;
    int64_t getImageWidth() const;
    int64_t getImageHeight() const;

private:
    ImageScene* m_imageScene = nullptr;
    QImage m_image;

    int64_t m_imageWidth = 1;
    int64_t m_imageHeight = 1;

signals:
    void messageBoxTrigger(QString messageTitle, QString messageText);

public slots:
    void updateImage(QImage image);
};

#endif // IMAGEVIEW_H
