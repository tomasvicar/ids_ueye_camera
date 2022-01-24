/*!
 * \file    mainwindow.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
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

#include "mainwindow.h"
#include "imageview.h"
#include "backend.h"

#include <QMessageBox>
#include <QAbstractButton>
#include <QApplication>

#include <cstdint>

#define VERSION "1.1.1"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // Create widget and layout
    QWidget* widget = new QWidget(this);
    m_layout = new QVBoxLayout(widget);
    widget->setLayout(m_layout);
    setCentralWidget(widget);

    m_backEnd = new BackEnd(this);

    // Connect the signal from the backend when an exception was thrown and a message should be printed
    // with the messagebox show slot in the MainWindow class
    connect(m_backEnd, &BackEnd::messageBoxTrigger, this, &MainWindow::showMessageBox);

    if (m_backEnd->start())
    {
        // Get the Sensor Size
        const int64_t imageWidth = m_backEnd->getImageWidth();
        const int64_t imageHeight = m_backEnd->getImageHeight();

        // Create ImageView to display the image
        m_imageView = new ImageView(widget, imageWidth, imageHeight);
        m_layout->addWidget(m_imageView);

        // Connect the signal from the image view when an exception was thrown and a message should be printed
        // with the messagebox show slot in the MainWindow class
        connect(m_imageView, &ImageView::messageBoxTrigger, this, &MainWindow::showMessageBox);

        // Connect the signal from the backend when a new image was received with the image update slot in
        // the ImageView class
        connect(m_backEnd, &BackEnd::imageReceived, m_imageView, &ImageView::updateImage);

        // Connect the signal from the backend when the counters have changed with the update slot in the
        // MainWindow class
        connect(m_backEnd, &BackEnd::countersUpdated, this, &MainWindow::updateCounters);

        createStatusBar();

        // Set minimum window size
        setMinimumSize(700, 500);

        m_hasError = false;
    }
    else
    {
        m_hasError = true;
    }
}

MainWindow::~MainWindow()
{
    delete m_backEnd;
    delete m_imageView;
    delete m_layout;
    delete m_labelInfo;
}

void MainWindow::createStatusBar()
{
    QWidget* statusBar = new QWidget(centralWidget());
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);

    m_labelInfo = new QLabel(statusBar);
    m_labelInfo->setAlignment(Qt::AlignLeft);
    layout->addWidget(m_labelInfo);
    layout->addStretch();

    QLabel* m_labelVersion = new QLabel(statusBar);
    m_labelVersion->setText(("chunks_live_qtwidgets v" VERSION));
    m_labelVersion->setAlignment(Qt::AlignRight);
    layout->addWidget(m_labelVersion);

    QLabel* m_labelAboutQt = new QLabel(statusBar);
    m_labelAboutQt->setObjectName("aboutQt");
    m_labelAboutQt->setText(R"(<a href="#aboutQt">About Qt</a>)");
    m_labelAboutQt->setAlignment(Qt::AlignRight);
    connect(m_labelAboutQt, SIGNAL(linkActivated(QString)), this, SLOT(on_aboutQt_linkActivated(QString)));
    layout->addWidget(m_labelAboutQt);
    statusBar->setLayout(layout);

    m_layout->addWidget(statusBar);
}

void MainWindow::updateCounters(unsigned int frameCounter, unsigned int errorCounter)
{
    QString strText;
    strText.sprintf("Acquired: %d, errors: %d", frameCounter, errorCounter);
    m_labelInfo->setText(strText);
}

void MainWindow::on_aboutQt_linkActivated(const QString& link)
{
    if (link == "#aboutQt")
    {
        QMessageBox::aboutQt(this, "About Qt");
    }
}

void MainWindow::showMessageBox(QString messageTitle, QString messageText)
{
    // Close existing messageboxes to avoid multiple messageboxes
    QWidgetList topWidgets = QApplication::topLevelWidgets();
    for (QWidget* w : topWidgets)
    {
        if (QMessageBox* mBox = qobject_cast<QMessageBox*>(w))
        {
            mBox->close();
        }
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle(messageTitle);
    msgBox.setText(messageText);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

bool MainWindow::hasError()
{
    return m_hasError;
}
