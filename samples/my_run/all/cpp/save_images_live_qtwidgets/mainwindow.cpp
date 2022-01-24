/*!
 * \file    mainwindow.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2019-05-01
 * \since   1.0.0
 *
 * \version 1.1.0
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

#include "mainwindow.h"

#include "acquisitionworker.h"
#include "display.h"

#include <peak_ipl/peak_ipl.hpp>

#include <peak/peak.hpp>

#include <QFileDialog>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QThread>
#include <QWidget>

#include <cstdint>
#include <iostream>

#define VERSION "1.1.1"


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    m_device = nullptr;
    m_display = nullptr;
    m_dataStream = nullptr;
    m_acquisitionWorker = nullptr;

    m_labelInfo = new QLabel(this);
    m_labelVersion = new QLabel(this);
    m_labelVersion->setText(("save_images_live_qtwidgets v" VERSION));
    m_labelVersion->setAlignment(Qt::AlignRight);
    m_labelInfo->setAlignment(Qt::AlignLeft);
    m_labelAboutQt = new QLabel(this);
    m_labelAboutQt->setObjectName("aboutQt");
    m_labelAboutQt->setText(R"(<a href="#aboutQt">About Qt</a>)");
    m_labelAboutQt->setAlignment(Qt::AlignRight);
    connect(m_labelAboutQt, SIGNAL(linkActivated(QString)), this, SLOT(on_aboutQt_linkActivated(QString)));

    m_buttonSave = new QPushButton("Press to Save Current Image", this);
    connect(m_buttonSave, SIGNAL(clicked()), this, SLOT(SaveImage()), Qt::UniqueConnection);

    // initialize peak library
    peak::Library::Initialize();

    if (OpenDevice())
    {
        try
        {
            // Create a display for the camera image
            m_display = new Display(this);

            // Create worker thread that waits for new images from the camera
            m_acquisitionWorker = new AcquisitionWorker();
            m_acquisitionWorker->SetDataStream(m_dataStream);
            m_acquisitionWorker->moveToThread(&m_acquisitionThread);

            // Connect the signal from the worker thread when a new image was received with the display update slot in
            // the Display class
            connect(m_acquisitionWorker, &AcquisitionWorker::imageReceived, m_display, &Display::onImageReceived);

            // Connect the signal from the worker thread when the counters have changed with the update slot in the
            // MainWindow class
            connect(m_acquisitionWorker, &AcquisitionWorker::counterUpdated, this, &MainWindow::onCounterUpdated);

            // Call start function of m_acquisitionWorker when thread starts
            connect(&m_acquisitionThread, &QThread::started, m_acquisitionWorker, &AcquisitionWorker::Start);

            // Start thread execution
            m_acquisitionThread.start();
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(this, "Exception", e.what(), QMessageBox::Ok);
        }
    }
    else
    {
        DestroyAll();
        exit(0);
    }

    // Set minimum window size
    this->setMinimumSize(700, 500);

    // Resize image display to the correct window size
    UpdateAllControlPositionsAndSizes();
}

MainWindow::~MainWindow()
{
    DestroyAll();
}

void MainWindow::DestroyAll()
{
    if (m_acquisitionWorker)
    {
        m_acquisitionWorker->Stop();
        m_acquisitionThread.quit();
        m_acquisitionThread.wait();

        delete m_acquisitionWorker;
        m_acquisitionWorker = nullptr;
    }

    CloseDevice();

    // close peak library
    peak::Library::Close();

    if (m_display)
    {
        delete m_display;
        m_display = nullptr;
    }

    if (m_labelInfo)
    {
        delete m_labelInfo;
        m_labelInfo = nullptr;
    }

    if (m_buttonSave)
    {
        delete m_buttonSave;
        m_buttonSave = nullptr;
    }

    if (m_labelVersion)
    {
        delete m_labelVersion;
        m_labelVersion = nullptr;
    }

    if (m_labelAboutQt)
    {
        delete m_labelAboutQt;
        m_labelAboutQt = nullptr;
    }
}

bool MainWindow::OpenDevice()
{
    try
    {
        // Create instance of the device manager
        auto& deviceManager = peak::DeviceManager::Instance();

        // Update the device manager
        deviceManager.Update();

        // Return if no device was found
        if (deviceManager.Devices().empty())
        {
            QMessageBox::critical(this, "Error", "No device found", QMessageBox::Ok);
            return false;
        }

        // open the first openable device in the device manager's device list
        size_t deviceCount = deviceManager.Devices().size();
        for (size_t i = 0; i < deviceCount; ++i)
        {
            if (deviceManager.Devices().at(i)->IsOpenable())
            {
                m_device = deviceManager.Devices().at(i)->OpenDevice(peak::core::DeviceAccessType::Control);

                // stop after the first opened device
                break;
            }
            else if (i == (deviceCount - 1))
            {
                QMessageBox::critical(this, "Error", "Device could not be opened", QMessageBox::Ok);
                return false;
            }
        }

        if (m_device)
        {
            // Open standard data stream
            auto dataStreams = m_device->DataStreams();
            if (dataStreams.empty())
            {
                QMessageBox::critical(this, "Error", "Device has no DataStream", QMessageBox::Ok);
                m_device.reset();
                return false;
            }

            m_dataStream = dataStreams.at(0)->OpenDataStream();

            // Get nodemap of remote device for all accesses to the genicam nodemap tree
            m_nodemapRemoteDevice = m_device->RemoteDevice()->NodeMaps().at(0);

            // To prepare for untriggered continuous image acquisition, load the default user set if available
            // and wait until execution is finished
            try
            {
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("UserSetSelector")
                    ->SetCurrentEntry("Default");

                m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->Execute();
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>("UserSetLoad")->WaitUntilDone();
            }
            catch (const peak::core::NotFoundException&)
            {
                // UserSet is not available
            }

            // Get the payload size for correct buffer allocation
            auto payloadSize = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("PayloadSize")->Value();

            // Get the minimum number of buffers that must be announced
            auto bufferCountMax = m_dataStream->NumBuffersAnnouncedMinRequired();

            // Allocate and announce image buffers and queue them
            for (size_t bufferCount = 0; bufferCount < bufferCountMax; ++bufferCount)
            {
                auto buffer = m_dataStream->AllocAndAnnounceBuffer(static_cast<size_t>(payloadSize), nullptr);
                m_dataStream->QueueBuffer(buffer);
            }

            return true;
        }
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, "Exception", e.what(), QMessageBox::Ok);
    }

    return false;
}

void MainWindow::CloseDevice()
{
    // if device was opened, try to stop acquisition
    if (m_device)
    {
        try
        {
            auto remoteNodeMap = m_device->RemoteDevice()->NodeMaps().at(0);
            remoteNodeMap->FindNode<peak::core::nodes::CommandNode>("AcquisitionStop")->Execute();
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(this, "Exception", e.what(), QMessageBox::Ok);
        }
    }

    // if data stream was opened, try to stop it and revoke its image buffers
    if (m_dataStream)
    {
        try
        {
            m_dataStream->KillWait();
            m_dataStream->StopAcquisition(peak::core::AcquisitionStopMode::Default);
            m_dataStream->Flush(peak::core::DataStreamFlushMode::DiscardAll);

            for (const auto& buffer : m_dataStream->AnnouncedBuffers())
            {
                m_dataStream->RevokeBuffer(buffer);
            }
        }
        catch (const std::exception& e)
        {
            QMessageBox::critical(this, "Exception", e.what(), QMessageBox::Ok);
        }
    }

    try
    {
        // Unlock parameters after acquisition stop
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("TLParamsLocked")->SetValue(0);
    }
    catch (const std::exception& e)
    {
        QMessageBox::critical(this, "Exception", e.what(), QMessageBox::Ok);
    }
}

void MainWindow::resizeEvent(QResizeEvent*)
{
    UpdateAllControlPositionsAndSizes();
}

std::string removeFileEndingIfExisting(const std::string& inputFileName)
{
    auto found = inputFileName.find_last_of('.');
    if (found == std::string::npos)
    {
        return inputFileName;
    }
    else
    {
        if (found == inputFileName.size() - 4 || found == inputFileName.size() - 5)
        {
            return inputFileName.substr(0, found);
        }
    }
    return inputFileName;
}

std::string MainWindow::selectSaveFileWithDialog()
{
    QFileDialog dialog(this);
    const std::string PNG_FILES = "PNG files (*.png)";
    const std::string JPEG_FILES = "JPEG files (*.jpg *.jpeg)";
    const std::string BMP_FILES = "BMP files (*.bmp)";
    dialog.setNameFilter(QString::fromStdString(PNG_FILES + ";;" + JPEG_FILES + ";;" + BMP_FILES));
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setOption(QFileDialog::DontUseNativeDialog);
    std::string stdfilename = "";
    if (dialog.exec())
    {
        auto qtfileNames = dialog.selectedFiles();
        if (!qtfileNames.empty())
        {
            stdfilename = qtfileNames.first().toStdString();
            // Remove the file ending if existing
            stdfilename = removeFileEndingIfExisting(stdfilename);
            const auto selectedFilter = dialog.selectedNameFilter().toStdString();
            if (selectedFilter == PNG_FILES)
            {
                stdfilename += ".png";
            }
            else if (selectedFilter == JPEG_FILES)
            {
                stdfilename += ".jpg";
            }
            else if (selectedFilter == BMP_FILES)
            {
                stdfilename += ".bmp";
            }
        }
    }
    return stdfilename;
}

void MainWindow::UpdateAllControlPositionsAndSizes()
{
    if (m_display)
    {
        m_display->setFixedSize(this->width(), this->height() - 30);
        m_display->move(0, 0);
    }

    if (m_labelInfo)
    {
        m_labelInfo->move(10, this->height() - 22);
        m_labelInfo->setFixedSize(200, 24);
    }

    if (m_labelVersion)
    {
        m_labelVersion->move(this->width() - 260, this->height() - 22);
        m_labelVersion->setFixedSize(200, 24);
    }

    if (m_labelAboutQt)
    {
        m_labelAboutQt->setFixedSize(70, 24);
        auto offsetWidth = m_labelAboutQt->width() + 10;
        auto offsetHeight = 22;
        m_labelAboutQt->move(this->width() - offsetWidth, this->height() - offsetHeight);
    }

    if (m_labelVersion)
    {
        auto offsetWidth = m_labelVersion->width();
        auto offsetHeight = 22;
        if (m_labelAboutQt)
        {
            offsetWidth += m_labelAboutQt->width() + 10;
        }
        m_labelVersion->move(this->width() - offsetWidth, this->height() - offsetHeight);
        m_labelVersion->setFixedSize(200, 24);
    }

    if (m_buttonSave)
    {
        auto offsetWidth = m_buttonSave->width();
        if (m_labelAboutQt)
        {
            offsetWidth += m_labelAboutQt->width() + 10;
        }
        if (m_labelVersion)
        {
            offsetWidth += m_labelVersion->width() + 10;
        }
        m_buttonSave->setFixedSize(200, 24);
        m_buttonSave->move(
            this->width() - offsetWidth, this->height() - m_buttonSave->height() - 2);
    }

}

void MainWindow::SaveImage()
{
    // We need this lock here to avoid an overwrite during saving
    m_writeMutex.lock();

    // Get and copy the current image
    auto currentQImage = m_display->getImage();

    peak::ipl::Image image(peak::ipl::PixelFormat(peak::ipl::PixelFormatName::BGRa8), currentQImage.bits(),
        static_cast<size_t>(currentQImage.byteCount()), static_cast<size_t>(currentQImage.width()),
        static_cast<size_t>(currentQImage.height()));

    auto newImage = image.Clone();

    // Unlock here so image capture can resume running
    m_writeMutex.unlock();

    auto stdfilename = selectSaveFileWithDialog();
    // Check if we did not get an empty filename

    if (!stdfilename.empty())
    {
        /*Write the file to disc. The format is chosen hereby from the filename
        Alternatively one can use e.g. WriteAsPNG to write it as PNG file but it requires the file ending to be png
        otherwise it will return an error. Using a specific function like WriteAsPNG one can specify Parameters e.g.
        compression/quality.
        */
        try
        {
            peak::ipl::ImageWriter::Write(stdfilename, newImage);
        }
        // Saving images may emit different exceptions e.g. if the application does not have the permissions to write
        // into a specific Folder an IO Exception will be thrown
        catch (const peak::ipl::ImageFormatNotSupportedException&)
        {
            QMessageBox::critical(this, QString("Error"),
                QString("Image format can not be written to selected File type. Try another. "), QMessageBox::Ok);
        }
        catch (const peak::ipl::IOException& e)
        {
            QMessageBox::critical(
                this, QString("Error"), QString("File IO Error: \n") + QString(e.what()), QMessageBox::Ok);
        }
        catch (const peak::ipl::Exception& e)
        {
            QMessageBox::critical(
                this, QString("Error"), QString("Internal processing Error: \n") + QString(e.what()), QMessageBox::Ok);
        }
    }
}

void MainWindow::onCounterUpdated(unsigned int frameCounter, unsigned int errorCounter)
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
