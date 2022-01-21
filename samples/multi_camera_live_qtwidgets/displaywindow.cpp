/*!
 * \file    displaywindow.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2021-03-15
 * \since   1.1.6
 *
 * \brief   The DisplayWindow class implements an easy way to display images from a
 *          camera in a Qt widgets window. It can be used for other QT widget
 *          applications as well.
 *
 * \version 1.1.0
 *
 * Copyright (C) 2020 - 2021, IDS Imaging Development Systems GmbH.
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

#include "displaywindow.h"

#include <peak_ipl/peak_ipl.hpp>

#include <peak/peak.hpp>

#include <QLabel>
#include <QLayout>
#include <QWidget>

#include <cmath>
#include <cstdint>


DisplayWindow::DisplayWindow(QWidget* parent, int posX, int posY, int desiredHeight, int imageWidth, int imageHeight,
    peak::ipl::PixelFormatName pixelFormat)
    : QWidget(parent)
{
    m_layout = new QVBoxLayout();
    this->setLayout(m_layout);

    int displayWidth = static_cast<int>(
        static_cast<double>(imageWidth) / static_cast<double>(imageHeight) * static_cast<double>(desiredHeight));

    this->setGeometry(posX, posY, displayWidth, desiredHeight);
    this->show();

    // Create a graphics view for the camera image
    m_graphicsView = new QGraphicsView(this);
    m_graphicsView->setStyleSheet("QGraphicsView { border-style: none; background-color: transparent }");
    m_scene = new CustomGraphicsScene(this);
    m_graphicsView->setScene(m_scene);

    // Create a label for the capture infos
    m_labelInfos = new QLabel();

    // Add the graphics display and the label to the corresponding layout of the display window
    m_layout->addWidget(m_graphicsView);
    m_layout->addWidget(m_labelInfos);
}


DisplayWindow::~DisplayWindow()
{
    if (m_labelInfos)
    {
        delete m_labelInfos;
        m_labelInfos = nullptr;
    }

    if (m_scene)
    {
        delete m_scene;
        m_scene = nullptr;
    }

    if (m_graphicsView)
    {
        delete m_graphicsView;
        m_graphicsView = nullptr;
    }

    if (m_layout)
    {
        delete m_layout;
        m_layout = nullptr;
    }
}


void DisplayWindow::UpdateDisplay(QImage image)
{
    m_scene->setImage(image);
}


void DisplayWindow::UpdateCounters(double frameTime_ms, double conversionTime_ms, unsigned int frameCounter,
    unsigned int errorCounter, int incomplete, int dropped, int lost, bool showCustomNodes)
{
    QString str;

    double frameRate_Hz = 0.0;
    if (frameTime_ms > 0)
    {
        frameRate_Hz = 1000.0 / frameTime_ms;
    }

    m_frameRate_Hz = AverageValue(m_frameRate_Hz, frameRate_Hz, 3.0);
    m_conversionTime_ms = AverageValue(m_conversionTime_ms, conversionTime_ms, 3.0);

    QString strText;

    if (showCustomNodes)
    {
        strText.sprintf(
            "Framerate: %.1f fps, conversion: %.1f ms, acquired: %d, errors: %d, incomplete: %d, dropped: %d, lost: %d",
            m_frameRate_Hz, m_conversionTime_ms, frameCounter, errorCounter, incomplete, dropped, lost);
    }
    else
    {
        strText.sprintf("Framerate: %.1f fps, conversion: %.1f ms, acquired: %d, errors: %d", m_frameRate_Hz,
            m_conversionTime_ms, frameCounter, errorCounter);
    }

    m_labelInfos->setText(strText);
}


double DisplayWindow::AverageValue(double val1, double val2, double deviation)
{
    double ret;

    if (((val1 - val2) > deviation) || ((val2 - val1) > deviation))
    {
        ret = val2;
    }
    else
    {
        ret = (15.0 * val1 + val2) / 16.0;
    }

    return ret;
}


CustomGraphicsScene::CustomGraphicsScene(DisplayWindow* parent)
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

    double imagePosX = -1.0 * (imageWidth / 2.0);
    double imagePosY = -1.0 * (imageHeight / 2.0);

    // Remove digits afer point
    imagePosX = trunc(imagePosX);
    imagePosY = trunc(imagePosY);

    QRectF rect(imagePosX, imagePosY, imageWidth, imageHeight);

    painter->drawImage(rect, m_image);
}
