/*!
 * \file    display.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2021-01-15
 * \since   1.2.0
 *
 * \brief   The Display class implements an easy way to display images from a
 *          camera in a QT widgets window. It can be used for other QT widget
 *          applications as well.
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

#include "display.h"

#include <QGraphicsView>
#include <QImage>
#include <QWidget>

#include <cmath>


Display::Display(QWidget* parent)
    : QGraphicsView(parent)
{
    m_scene = new CustomGraphicsScene(this);
    this->setScene(m_scene);
}


Display::~Display()
{}


void Display::onImageReceived(QImage image)
{
    m_scene->setImage(image);
}


CustomGraphicsScene::CustomGraphicsScene(Display* parent)
    : QGraphicsScene(parent)
    , m_parent(parent)
{}


CustomGraphicsScene::~CustomGraphicsScene()
{}


void CustomGraphicsScene::setImage(QImage image)
{
    m_image = image;
    update();
}


void CustomGraphicsScene::drawBackground(QPainter* painter, const QRectF&)
{
    // Display size
    double displayWidth = static_cast<double>(m_parent->width());
    double displayHeight = static_cast<double>(m_parent->height());

    // Image size
    double imageWidth = static_cast<double>(m_image.width());
    double imageHeight = static_cast<double>(m_image.height());

    // Calculate aspect ratio of the display
    double ratio1 = displayWidth / displayHeight;

    // Calculate aspect ratio of the image
    double ratio2 = imageWidth / imageHeight;

    if (ratio1 > ratio2)
    {
        // the height with must fit to the display height. So h remains and w must be scaled down
        imageWidth = displayHeight * ratio2;
        imageHeight = displayHeight;
    }
    else
    {
        // the image with must fit to the display width. So w remains and h must be scaled down
        imageWidth = displayWidth;
        imageHeight = displayWidth / ratio2;
    }

    double posX = -1.0 * (displayWidth / 2.0);
    double posY = -1.0 * (displayHeight / 2.0);

    QRectF rect(trunc(posX), trunc(posY), displayWidth, displayHeight);
    painter->fillRect(rect, palette().color(QPalette::Background));

    posX = -1.0 * (imageWidth / 2.0);
    posY = -1.0 * (imageHeight / 2.0);

    rect = QRectF(trunc(posX), trunc(posY), imageWidth, imageHeight);
    painter->drawImage(rect, m_image);
}
