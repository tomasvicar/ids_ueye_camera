/*!
 * \file    mainwindow.h
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "acquisitionworker.h"
#include "display.h"

#include <peak_ipl/peak_ipl.hpp>

#include <peak/peak.hpp>

#include <QBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QThread>
#include <QWidget>


#include <cstdint>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    std::shared_ptr<peak::core::Device> m_device;
    std::shared_ptr<peak::core::DataStream> m_dataStream;
    std::shared_ptr<peak::core::NodeMap> m_nodemapRemoteDevice;

    Display* m_display;
    QLabel* m_labelInfo;
    QLabel* m_labelVersion;
    QPushButton* m_buttonSave;
    QLabel* m_labelAboutQt;

    AcquisitionWorker* m_acquisitionWorker;
    QThread m_acquisitionThread;

    std::mutex m_writeMutex;

    void DestroyAll();

    bool OpenDevice();
    void CloseDevice();

    void UpdateAllControlPositionsAndSizes();

    void resizeEvent(QResizeEvent* e);

    std::string selectSaveFileWithDialog();

public slots:
    void SaveImage();
    void onCounterUpdated(unsigned int frameCounter, unsigned int errorCounter);
    void on_aboutQt_linkActivated(const QString& link);
};

#endif // MAINWINDOW_H
