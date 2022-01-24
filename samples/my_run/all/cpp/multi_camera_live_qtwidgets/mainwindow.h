/*!
 * \file    mainwindow.h
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \version 1.0.0
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "acquisitionworker.h"
#include "displaywindow.h"

#include <peak_ipl/peak_ipl.hpp>

#include <peak/peak.hpp>

#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QThread>
#include <QVBoxLayout>
#include <QWidget>

#include <cstdint>


// The members of this structure are important for the use of the camera
struct DeviceContext
{
    std::shared_ptr<peak::core::Device> device;
    std::shared_ptr<peak::core::DataStream> dataStream;
    std::shared_ptr<peak::core::NodeMap> nodemapRemoteDevice;
    peak::ipl::PixelFormatName pixelFormat;
    DisplayWindow* displayWindow;
    int imageWidth;
    int imageHeight;
    AcquisitionWorker* acquisitionWorker;
    QThread acquisitionThread;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    std::vector<std::shared_ptr<DeviceContext>> m_vecDevices;

    QWidget* m_centralWidget;
    QVBoxLayout* m_layout;
    QWidget* m_statusBar;
    QHBoxLayout* m_statusBarLayout;
    QLabel* m_statusBarLabelVersion;
    QLabel* m_statusBarLabelAboutQt;

    void DestroyAll();

    bool OpenDevices();
    void CloseDevices();
    void CreateStatusBar();

    void closeEvent(QCloseEvent* event);

public slots:

    void OnAboutQt(const QString& link);
};

#endif // MAINWINDOW_H
