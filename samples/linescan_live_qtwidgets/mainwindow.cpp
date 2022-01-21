/*!
 * \file    mainwindow.cpp
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

#include "mainwindow.h"
#include "backend.h"
#include "imageview.h"

#include <QAbstractButton>
#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QSizePolicy>
#include <QSpacerItem>

#include <cstdint>

#define VERSION "1.0.1"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    m_lastError = "-";

    m_backEnd = new BackEnd(this);

    // Create widget and layout
    QWidget* m_widget = new QWidget(this);

    m_configLayout = new QVBoxLayout;
    m_imageLayout = new QVBoxLayout;

    m_windowLayout = new QHBoxLayout(m_widget);
    m_windowLayout->addLayout(m_configLayout);
    m_windowLayout->addLayout(m_imageLayout);

    m_widget->setLayout(m_windowLayout);
    setCentralWidget(m_widget);

    // Connect the signal from the backend when an exception was thrown and a message should be printed
    // with the messagebox show slot in the MainWindow class
    connect(m_backEnd, &BackEnd::messageBoxTrigger, this, &MainWindow::showMessageBox);

	connect(m_backEnd, &BackEnd::cameraDisconnected, this, &MainWindow::onCameraDisconnect);

    connect(m_backEnd, &BackEnd::errorOccurred, this, &MainWindow::updateLastError);

    connect(m_backEnd, &BackEnd::started, this, &MainWindow::init);

    QEventLoop loop;
    connect(this, &MainWindow::initialized, &loop, &QEventLoop::quit);

    if (m_backEnd->start())
    {
        loop.exec();
    }
}

MainWindow::~MainWindow()
{
    delete m_backEnd;
    delete m_imageView;
    delete m_imageLayout;
    delete m_labelInfo;
}

void MainWindow::init(bool backendStarted)
{
    if (backendStarted == true)
    {
        // Get the Sensor Size
        const int64_t imageWidth = m_backEnd->getImageWidth();
        const int64_t imageHeight = m_backEnd->getImageHeight();

        // Create ImageView to display the image
        m_imageView = new ImageView(m_widget, imageWidth, imageHeight);
        m_imageLayout->addWidget(m_imageView);

        // Connect the signal from the image view when an exception was thrown and a message should be printed
        // with the messagebox show slot in the MainWindow class
        connect(m_imageView, &ImageView::messageBoxTrigger, this, &MainWindow::showMessageBox);

        // Connect the signal from the backend when a new image was received with the image update slot in
        // the ImageView class
        connect(m_backEnd, &BackEnd::imageReceived, m_imageView, &ImageView::updateImage);

        // Connect the signal from the backend when the counters have changed with the update slot in the
        // MainWindow class
        connect(m_backEnd, &BackEnd::countersUpdated, this, &MainWindow::updateStatusBar);

        createConfigMenu();
        createStatusBar();

        // Set minimum window size
        setMinimumSize(1000, 500);
        m_hasError = false;
    }
    else
    {
        m_hasError = true;
    }
    emit initialized();
}

void MainWindow::createConfigMenu()
{
    auto nodemapRemoteDevice = m_backEnd->getNodemapRemoteDevice();

    bool frameStartTriggerMode = m_backEnd->getFrameStartTrigger().first;
    std::string frameStartTriggerSource = m_backEnd->getFrameStartTrigger().second.first.toUtf8().constData();
    std::string frameStartTriggerActivation = m_backEnd->getFrameStartTrigger().second.first.toUtf8().constData();

    bool lineStartTriggerMode = m_backEnd->getLineStartTrigger().first;
    std::string lineStartTriggerSource = m_backEnd->getLineStartTrigger().second.first.toUtf8().constData();
    std::string lineStartTriggerActivation = m_backEnd->getLineStartTrigger().second.first.toUtf8().constData();

    // Create groupbox for offsetY property control
    m_offsetYGroupBox = new QGroupBox("ScanLine offset");
    m_offsetY = new PropertyControlInteger("OffsetY", nodemapRemoteDevice, this);
    m_offsetYGroupBox->setLayout(m_offsetY->getLayout());
    m_configLayout->addWidget(m_offsetYGroupBox);
	connect(m_offsetY, &PropertyControlInteger::cameraDisconnected, this, &MainWindow::onCameraDisconnect);

    if (frameStartTriggerMode && frameStartTriggerSource == "Software")
    {
        // Create groupbox for trigger software property control
        m_triggerSoftwareGroupBox = new QGroupBox("FrameStarttrigger");
        m_triggerSoftware = new PropertyControlCommand("TriggerSoftware", nodemapRemoteDevice, this);
        m_triggerSoftwareGroupBox->setLayout(m_triggerSoftware->getLayout());
        m_configLayout->addWidget(m_triggerSoftwareGroupBox);
        connect(m_triggerSoftware, &PropertyControlCommand::buttonPressed, m_backEnd, &BackEnd::triggerExecuted);
		connect(m_triggerSoftware, &PropertyControlCommand::cameraDisconnected, this, &MainWindow::onCameraDisconnect);
    }

    if (!lineStartTriggerMode)
    {
        // Create groupbox for acquisition linerate property control
        m_acquisitionLineRateGroupBox = new QGroupBox("LineRate");
        m_acquisitionLineRate = new PropertyControlFloat("AcquisitionLineRate", nodemapRemoteDevice, this);
        m_acquisitionLineRateGroupBox->setLayout(m_acquisitionLineRate->getLayout());
        m_configLayout->addWidget(m_acquisitionLineRateGroupBox);
		connect(m_acquisitionLineRate, &PropertyControlFloat::cameraDisconnected, this, &MainWindow::onCameraDisconnect);
    }

    if ((frameStartTriggerMode && frameStartTriggerSource == "PWM0")
        || (lineStartTriggerMode && lineStartTriggerSource == "PWM0"))
    {
        // Create groupbox for PWM property control
        m_PWMFrequencyGroupBox = new QGroupBox("PWM");
        m_PWMFrequency = new PropertyControlFloat("PWMFrequency", nodemapRemoteDevice, this);
        m_PWMFrequencyGroupBox->setLayout(m_PWMFrequency->getLayout());
        m_configLayout->addWidget(m_PWMFrequencyGroupBox);
		connect(m_PWMFrequency, &PropertyControlFloat::cameraDisconnected, this, &MainWindow::onCameraDisconnect);
    }

    // Create groupbox for gain property control
    m_gainGroupBox = new QGroupBox("Gain", this);

    QStringList gainSelectorItems;
    auto gainSelectors = nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("GainSelector")->Entries();
    for (auto& gainSelector : gainSelectors)
    {
        gainSelectorItems << QString::fromStdString(gainSelector->StringValue());
    }

    m_gainSelector = new PropertyControlEnum("GainSelector", nodemapRemoteDevice, gainSelectorItems, this);
    m_gain = new PropertyControlFloat("Gain", nodemapRemoteDevice, this);
    QVBoxLayout* gainLayout = new QVBoxLayout;
    gainLayout->addLayout(m_gainSelector->getLayout());
    gainLayout->addLayout(m_gain->getLayout());
    m_gainGroupBox->setLayout(gainLayout);
    m_configLayout->addWidget(m_gainGroupBox);
    connect(m_gainSelector, &PropertyControlEnum::valueChanged, m_gain, &PropertyControlFloat::update);
	connect(m_gain, &PropertyControlFloat::cameraDisconnected, this, &MainWindow::onCameraDisconnect);
	connect(m_gainSelector, &PropertyControlEnum::cameraDisconnected, this, &MainWindow::onCameraDisconnect);

    QSpacerItem* spacer = new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::Expanding);
    m_configLayout->addSpacerItem(spacer);
}

void MainWindow::createStatusBar()
{
    QWidget* statusBar = new QWidget(centralWidget());
	QVBoxLayout* statusBarLayout = new QVBoxLayout(statusBar);
	QWidget* statusBarFirstLine = new QWidget(statusBar);
	QWidget* statusBarSecondLine = new QWidget(statusBar);
    QHBoxLayout* firstLinelayout = new QHBoxLayout(statusBarFirstLine);
	QHBoxLayout* secondLinelayout = new QHBoxLayout(statusBarSecondLine);

	statusBarLayout->addWidget(statusBarFirstLine);
	statusBarLayout->addWidget(statusBarSecondLine);
	statusBarFirstLine->setLayout(firstLinelayout);
	statusBarSecondLine->setLayout(secondLinelayout);

	statusBarLayout->setContentsMargins(0, 0, 0, 0);
	firstLinelayout->setContentsMargins(0, 0, 0, 0);
	secondLinelayout->setContentsMargins(0, 0, 0, 0);

    m_labelInfo = new QLabel(statusBar);
    m_labelInfo->setText("Acquired: 0, errors: 0, Framerate: -, Linerate: -");
    m_labelInfo->setAlignment(Qt::AlignLeft);
	firstLinelayout->addWidget(m_labelInfo);
	firstLinelayout->addStretch();

    QLabel* m_labelVersion = new QLabel(statusBar);
    m_labelVersion->setText(("linescan_live_qtwidgets v" VERSION));
    m_labelVersion->setAlignment(Qt::AlignRight);
	firstLinelayout->addWidget(m_labelVersion);

    QLabel* m_labelAboutQt = new QLabel(statusBar);
    m_labelAboutQt->setObjectName("aboutQt");
    m_labelAboutQt->setText(R"(<a href="#aboutQt">About Qt</a>)");
    m_labelAboutQt->setAlignment(Qt::AlignRight);
    connect(m_labelAboutQt, SIGNAL(linkActivated(QString)), this, SLOT(on_aboutQt_linkActivated(QString)));
	firstLinelayout->addWidget(m_labelAboutQt);

    m_errorLabel = new QLabel(this);
    m_errorLabel->setAlignment(Qt::AlignLeft);
    m_errorLabel->setText("Errors: 0, Last error:");
	secondLinelayout->addWidget(m_errorLabel);

    m_lastErrorLabel = new QLabel(this);
    m_lastErrorLabel->setText("-");
    m_lastErrorLabel->setAlignment(Qt::AlignLeft);
    m_lastErrorLabel->setWordWrap(true);
    m_lastErrorLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	secondLinelayout->addWidget(m_lastErrorLabel);

    m_imageLayout->addWidget(statusBar);
}

void MainWindow::updateStatusBar(
    unsigned int frameCounter, unsigned int errorCounter, double frameRate_fps, double lineRate_lps)
{
    QString strText;
	strText = QString("Acquired: %1, ").arg(frameCounter);
    strText.append(frameRate_fps < 0 ? QString("Framerate: not available, ") :
                                       QString("Framerate: %1 fps, ").arg(QString::number(frameRate_fps, 'f', 2)));
    strText.append(lineRate_lps < 0 ? QString("Linerate: not available") :
                                      QString("Linerate: %1 lps").arg(QString::number(lineRate_lps, 'f', 2)));

    m_errorLabel->setText(QString("Errors: %1, Last error: ").arg(QString::number(errorCounter)));
    m_lastErrorLabel->setText(m_lastError);

    m_labelInfo->setText(strText);
}

void MainWindow::updateLastError(QString errorText)
{
    m_lastError = errorText;
}

void MainWindow::on_aboutQt_linkActivated(const QString& link)
{
    if (link == "#aboutQt")
    {
        QMessageBox::aboutQt(this, "About Qt");
    }
}

void MainWindow::onCameraDisconnect()
{
	QMessageBox msgBox;
	msgBox.setWindowTitle("Internal Error");
	msgBox.setText("An internal error occurred! The reason for that could be a connection issue.\nMake sure that the device is still connected and restart the application.");
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setDefaultButton(QMessageBox::Ok);
	msgBox.exec();

	QApplication::exit(-1);
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
