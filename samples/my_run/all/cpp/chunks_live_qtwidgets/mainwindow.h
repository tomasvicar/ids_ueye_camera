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

#include <QMainWindow>
#include <QVBoxLayout>
#include <QThread>
#include <QLabel>
#include <QString>

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
    ImageView* m_imageView = nullptr;
    QVBoxLayout* m_layout = nullptr;
    QLabel* m_labelInfo = nullptr;

    bool m_hasError = true;

    void createStatusBar();

private slots:
    void updateCounters(unsigned int frameCounter, unsigned int errorCounter);
    void on_aboutQt_linkActivated(const QString& link);
    void showMessageBox(QString messageTitle, QString messageText);
};
#endif // MAINWINDOW_H
