/*!
 * \file    display.h
 * \author  IDS Imaging Development Systems GmbH
 * \date    2019-05-01
 * \since   1.0.0
 *
 * \brief   The Display class implements an easy way to display images from a
 *          camera in a QT widgets window. It can be used for other QT widget
 *          applications as well.
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

#ifndef DISPLAY_H
#define DISPLAY_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QRect>

#include <cstdint>


class Display;


class CustomGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    CustomGraphicsScene(Display* pParent);
    ~CustomGraphicsScene();

    void setImage(QImage image);

private:
    Display* m_parent;
    QImage m_image;

    virtual void drawBackground(QPainter* painter, const QRectF& rect);
};


class Display : public QGraphicsView
{
    Q_OBJECT

public:
    Display(QWidget* parent);
    ~Display();

private:
    CustomGraphicsScene* m_scene;

public slots:
    void onImageReceived(QImage image);
};

#endif // DISPLAY_H
