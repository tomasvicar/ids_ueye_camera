/*!
 * \file    displaywindow.h
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

#ifndef DISPLAY_WINDOW_H
#define DISPLAY_WINDOW_H

#include <peak_ipl/peak_ipl.hpp>

#include <peak/peak.hpp>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QLayout>
#include <QPainter>
#include <QRect>
#include <QWidget>

#include <cstdint>


class DisplayWindow;


class CustomGraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    CustomGraphicsScene(DisplayWindow* pParent);
    ~CustomGraphicsScene();

    void setImage(QImage image);

private:
    DisplayWindow* m_parent;
    QImage m_image;

    virtual void drawBackground(QPainter* painter, const QRectF& rect);
};


class DisplayWindow : public QWidget
{
    Q_OBJECT

public:
    DisplayWindow(QWidget* parent, int posX, int posY, int desiredHeight, int imageWidth, int imageHeight,
        peak::ipl::PixelFormatName pixelFormat);
    ~DisplayWindow();

private:
    QLabel* m_labelInfos;
    QVBoxLayout* m_layout;

    QGraphicsView* m_graphicsView;
    CustomGraphicsScene* m_scene;

    double m_frameRate_Hz;
    double m_conversionTime_ms;

    double AverageValue(double val1, double val2, double deviation);

public slots:
    void UpdateDisplay(QImage image);
    void UpdateCounters(double frameTime_ms, double conversionTime_ms, unsigned int frameCounter,
        unsigned int errorCounter, int incomplete, int dropped, int lost, bool showCustomNodes);
};

#endif // DISPLAY_WINDOW_H
