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

#include "propertycontrolcommand.h"
#include "propertycontrolenum.h"
#include "propertycontrolfloat.h"
#include "propertycontrolinteger.h"

#include <QGroupBox>
#include <QLabel>
#include <QMainWindow>
#include <QString>
#include <QThread>
#include <QVBoxLayout>

class BackEnd;
class ImageView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    bool hasError();

private:
    BackEnd* m_backEnd = nullptr;
    QWidget* m_widget = nullptr;
    ImageView* m_imageView = nullptr;
    QLabel* m_labelInfo = nullptr;
    QLabel* m_errorLabel = nullptr;
    QLabel* m_lastErrorLabel = nullptr;
    QHBoxLayout* m_windowLayout = nullptr;
    QVBoxLayout* m_imageLayout = nullptr;
    QVBoxLayout* m_configLayout = nullptr;

    PropertyControlInteger* m_offsetY = nullptr;
    PropertyControlCommand* m_triggerSoftware = nullptr;
    PropertyControlFloat* m_acquisitionLineRate = nullptr;
    PropertyControlFloat* m_PWMFrequency = nullptr;
    PropertyControlEnum* m_gainSelector = nullptr;
    PropertyControlFloat* m_gain = nullptr;

    QGroupBox* m_offsetYGroupBox = nullptr;
    QGroupBox* m_triggerSoftwareGroupBox = nullptr;
    QGroupBox* m_acquisitionLineRateGroupBox = nullptr;
    QGroupBox* m_PWMFrequencyGroupBox = nullptr;
    QGroupBox* m_gainGroupBox = nullptr;

    QString m_lastError = "";


    bool m_hasError = true;

    void createConfigMenu();
    void createStatusBar();

signals:
    void initialized();

private slots:
    void updateStatusBar(unsigned int frameCounter, unsigned int errorCounter, double frameRate, double lineRate);
    void updateLastError(QString errorText);
    void on_aboutQt_linkActivated(const QString& link);
    void showMessageBox(QString messageTitle, QString messageText);
	void onCameraDisconnect();
    void init(bool backendStarted);
};
#endif // MAINWINDOW_H
