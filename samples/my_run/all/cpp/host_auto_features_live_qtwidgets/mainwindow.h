/*!
 * \file    mainwindow.h
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "acquisitionworker.h"
#include "display.h"

#include <peak_ipl/peak_ipl.hpp>

#include <peak/peak.hpp>

#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QRadioButton>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QSpinBox>

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
    AutoFeaturesManager m_autoFeaturesManager;

    bool m_gainAutoSupported;
    bool m_balanceWhiteAutoSupported;

    QWidget* m_centralWidget;

    QLabel* m_labelInfo;
    QLabel* m_labelVersion;
    QLabel* m_labelAboutQt;
    QHBoxLayout* m_layoutStatusBar;
    QWidget* m_statusBar;

    Display* m_display;

    // Controls for ExposureAuto
    QLabel* m_labelExposureAuto;
    QLabel* m_labelExposureAutoOff;
    QRadioButton* m_radioExposureAutoOff;
    QLabel* m_labelExposureAutoOnce;
    QRadioButton* m_radioExposureAutoOnce;
    QLabel* m_labelExposureAutoContinuous;
    QRadioButton* m_radioExposureAutoContinuous;
    QButtonGroup* m_groupExposureAuto;

    // Controls for GainAuto
    QLabel* m_labelGainAuto;
    QLabel* m_labelGainAutoOff;
    QRadioButton* m_radioGainAutoOff;
    QLabel* m_labelGainAutoOnce;
    QRadioButton* m_radioGainAutoOnce;
    QLabel* m_labelGainAutoContinuous;
    QRadioButton* m_radioGainAutoContinuous;
    QButtonGroup* m_groupGainAuto;

    // Controls for BalanceWhiteAuto
    QLabel* m_labelBalanceWhiteAuto;
    QLabel* m_labelBalanceWhiteAutoOff;
    QRadioButton* m_radioBalanceWhiteAutoOff;
    QLabel* m_labelBalanceWhiteAutoOnce;
    QRadioButton* m_radioBalanceWhiteAutoOnce;
    QLabel* m_labelBalanceWhiteAutoContinuous;
    QRadioButton* m_radioBalanceWhiteAutoContinuous;
    QButtonGroup* m_groupBalanceWhiteAuto;

    QPushButton* m_buttonReset;

    QLabel* m_labelSkipFrames;
    QSpinBox* m_spinBoxSkipFrames;

    QHBoxLayout* m_layoutRadioButtonsExposureAuto;
    QHBoxLayout* m_layoutRadioButtonsGainAuto;
    QHBoxLayout* m_layoutRadioButtonsBalanceWhiteAuto;
    QHBoxLayout* m_layoutSkipFramesControls;
    QGridLayout* m_layoutControls;
    QVBoxLayout* m_layout;

    AcquisitionWorker* m_acquisitionWorker;
    QThread m_acquisitionThread;

    double m_frameRateSet;

    bool OpenDevice();
    void CloseDevice();

    void CreateControls();
    void DestroyAll();

    void DeleteObject(QObject* object);
    void DeleteLayout(QLayout* layout);

public slots:

    void OnCounterUpdated(unsigned int frameCounter, unsigned int errorCounter);
    void OnAboutQtLinkActivated(const QString& link);
    void OnRadioExposureAuto(int mode);
    void OnRadioGainAuto(int mode);
    void OnRadioBalanceWhiteAuto(int mode);
    void OnButtonReset();
    void OnSpinBoxSkipFrames(int skipFrames);
    void UpdateAutoControls();
};

#endif // MAINWINDOW_H
