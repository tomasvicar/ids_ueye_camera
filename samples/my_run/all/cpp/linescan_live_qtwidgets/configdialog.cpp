/*!
 * \file    configdialog.cpp
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

#include "configdialog.h"

#include <QDebug>
#include <QtGlobal>

ConfigDialog::ConfigDialog(std::shared_ptr<peak::core::NodeMap> nodeMap, BackEnd* parent)
{
    m_parent = parent;
    m_nodemapRemoteDevice = nodeMap;
    setWindowTitle("Linescan Configuration");
    setAttribute(Qt::WA_DeleteOnClose);

    m_vLayout = new QVBoxLayout(this);

    createPropertyControls();

    m_dialogButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(m_dialogButtons, &QDialogButtonBox::accepted, this, &ConfigDialog::applyConfig);
    connect(m_dialogButtons, &QDialogButtonBox::rejected, this, &ConfigDialog::dialogRejected);
    m_vLayout->addWidget(m_dialogButtons);
    setLayout(m_vLayout);
}

void ConfigDialog::applyConfig()
{
    int offsetY = m_offsetY->getValue();
    int imageHeight = m_height->getValue();

    std::pair<bool, std::pair<QString, QString>> frameStartTrigger;
    std::pair<bool, std::pair<QString, QString>> lineStartTrigger;

    frameStartTrigger.first = m_frameStartTriggerMode->getValue();
    frameStartTrigger.second.first = m_frameStartTriggerSource->getValue();
    frameStartTrigger.second.second = m_frameStartTriggerActivation->getValue();

    lineStartTrigger.first = m_lineStartTriggerMode->getValue();
    lineStartTrigger.second.first = m_lineStartTriggerSource->getValue();
    lineStartTrigger.second.second = m_lineStartTriggerActivation->getValue();

    int acquisitionLineRate = m_acquisitionLineRate->getValue();
    int PWMFrequency = m_PWMFrequency->getValue();

    emit configSubmitted(offsetY, imageHeight, frameStartTrigger, lineStartTrigger, acquisitionLineRate, PWMFrequency);
}

void ConfigDialog::createPropertyControls()
{
    try
    {
        QStringList frameStartTriggerSourceItems = QStringList() << "Software"
                                                                 << "Line0"
                                                                 << "Line2"
                                                                 << "Line3"
                                                                 << "PWM0";
        QStringList lineStartTriggerSourceItems = QStringList() << "Line2"
                                                                << "Line3"
                                                                << "PWM0";

        // Create control widget for every property to configure
        m_offsetY = new PropertyControlInteger("OffsetY", m_nodemapRemoteDevice, this);
        m_height = new PropertyControlInteger("Height", m_nodemapRemoteDevice, this);
        m_frameStartTriggerMode = new PropertyControlBool("TriggerMode", m_nodemapRemoteDevice, this, "FrameStart");
        m_frameStartTriggerSource = new PropertyControlEnum(
            "TriggerSource", m_nodemapRemoteDevice, frameStartTriggerSourceItems, this, "FrameStart");
        m_frameStartTriggerActivation = new PropertyControlEnum(
            "TriggerActivation", m_nodemapRemoteDevice, QStringList(), this, "FrameStart");
        m_lineStartTriggerMode = new PropertyControlBool("TriggerMode", m_nodemapRemoteDevice, this, "LineStart");
        m_lineStartTriggerSource = new PropertyControlEnum(
            "TriggerSource", m_nodemapRemoteDevice, lineStartTriggerSourceItems, this, "LineStart");
        m_lineStartTriggerActivation = new PropertyControlEnum(
            "TriggerActivation", m_nodemapRemoteDevice, QStringList(), this, "LineStart");
        m_PWMFrequency = new PropertyControlFloat("PWMFrequency", m_nodemapRemoteDevice, this);
        m_acquisitionLineRate = new PropertyControlFloat("AcquisitionLineRate", m_nodemapRemoteDevice, this);

        // Create groupbox for offsetY property control
        m_offsetGroupBox = new QGroupBox("ScanLine offset", this);
        auto maxOffsetY = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>("OffsetY")
                              ->Maximum();
        m_offsetY->setValue(maxOffsetY % 2);
        m_offsetGroupBox->setLayout(m_offsetY->getLayout());
        m_vLayout->addWidget(m_offsetGroupBox);

        // Create groupbox for height property control
        m_heightGroupBox = new QGroupBox("Height of composed image", this);
        m_height->setValue(1024);
        m_heightGroupBox->setLayout(m_height->getLayout());
        m_vLayout->addWidget(m_heightGroupBox);

        // Create groupbox for the framestart trigger control
        m_frameStartGroupBox = new QGroupBox("FrameStart trigger", this);
        QVBoxLayout* frameStartGroupBoxLayout = new QVBoxLayout(m_frameStartGroupBox);
        m_frameStartTriggerSource->setEnabled(false);
        connect(m_frameStartTriggerSource, &PropertyControlEnum::valueChanged, this,
            &ConfigDialog::handleTriggerSourceChanged);
        m_frameStartTriggerActivation->setEnabled(false);
        connect(
            m_frameStartTriggerMode, &PropertyControlBool::valueChanged, this, &ConfigDialog::handleTriggerModeChanged);
        frameStartGroupBoxLayout->addLayout(m_frameStartTriggerMode->getLayout());
        frameStartGroupBoxLayout->addLayout(m_frameStartTriggerSource->getLayout());
        frameStartGroupBoxLayout->addLayout(m_frameStartTriggerActivation->getLayout());
        m_frameStartGroupBox->setLayout(frameStartGroupBoxLayout);
        m_vLayout->addWidget(m_frameStartGroupBox);

        // Create groupbox for the linestart trigger control
        m_lineStartGroupBox = new QGroupBox("LineStart trigger", this);
        QVBoxLayout* lineStartGroupBoxLayout = new QVBoxLayout(m_lineStartGroupBox);
        auto maxAcquisitionLineRate =
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("AcquisitionLineRate")->Maximum();
        m_acquisitionLineRate->setValue(maxAcquisitionLineRate);
        m_lineStartTriggerSource->setEnabled(false);
        connect(m_lineStartTriggerSource, &PropertyControlEnum::valueChanged, this,
            &ConfigDialog::handleTriggerSourceChanged);
        m_lineStartTriggerActivation->setEnabled(false);
        connect(
            m_lineStartTriggerMode, &PropertyControlBool::valueChanged, this, &ConfigDialog::handleTriggerModeChanged);
        lineStartGroupBoxLayout->addLayout(m_lineStartTriggerMode->getLayout());
        lineStartGroupBoxLayout->addLayout(m_acquisitionLineRate->getLayout());
        lineStartGroupBoxLayout->addLayout(m_lineStartTriggerSource->getLayout());
        lineStartGroupBoxLayout->addLayout(m_lineStartTriggerActivation->getLayout());
        m_lineStartGroupBox->setLayout(lineStartGroupBoxLayout);
        m_vLayout->addWidget(m_lineStartGroupBox);


        // Create groupbox for PWM property control
        m_PWMGroupBox = new QGroupBox("PWM", this);
        m_PWMFrequency->setEnabled(false);
        auto maxAcquisitionFrameRate =
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("AcquisitionFrameRate")->Maximum();
        auto maxPWMFrequency = m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>("PWMFrequency")
                                   ->Maximum();
        m_PWMFrequency->setValue(std::min(maxAcquisitionFrameRate, maxPWMFrequency));
        m_PWMGroupBox->setLayout(m_PWMFrequency->getLayout());
        m_vLayout->addWidget(m_PWMGroupBox);

        // Connect all the messageBoxTrigger from the property controls with the messageBoxTrigger of the ConfigDialog
        connect(m_offsetY, &PropertyControlInteger::messageBoxTrigger, this, &ConfigDialog::messageBoxTrigger);
        connect(m_height, &PropertyControlInteger::messageBoxTrigger, this, &ConfigDialog::messageBoxTrigger);
        connect(m_frameStartTriggerMode, &PropertyControlBool::messageBoxTrigger, this, &ConfigDialog::messageBoxTrigger);
        connect(m_frameStartTriggerSource, &PropertyControlEnum::messageBoxTrigger, this, &ConfigDialog::messageBoxTrigger);
        connect(m_frameStartTriggerActivation, &PropertyControlEnum::messageBoxTrigger, this, &ConfigDialog::messageBoxTrigger);
        connect(m_lineStartTriggerMode, &PropertyControlBool::messageBoxTrigger, this, &ConfigDialog::messageBoxTrigger);
        connect(m_lineStartTriggerSource, &PropertyControlEnum::messageBoxTrigger, this, &ConfigDialog::messageBoxTrigger);
        connect(m_lineStartTriggerActivation, &PropertyControlEnum::messageBoxTrigger, this, &ConfigDialog::messageBoxTrigger);
        connect(m_PWMFrequency, &PropertyControlFloat::messageBoxTrigger, this, &ConfigDialog::messageBoxTrigger);
        connect(m_acquisitionLineRate, &PropertyControlFloat::messageBoxTrigger, this, &ConfigDialog::messageBoxTrigger);

        // Connect all the cameraDisconnect's from the property controls with the cameraDisconnectTrigger of the ConfigDialog
        connect(m_offsetY, &PropertyControlInteger::cameraDisconnected, this, &ConfigDialog::cameraDisconnected);
        connect(m_height, &PropertyControlInteger::cameraDisconnected, this, &ConfigDialog::cameraDisconnected);
        connect(m_frameStartTriggerMode, &PropertyControlBool::cameraDisconnected, this, &ConfigDialog::cameraDisconnected);
        connect(m_frameStartTriggerSource, &PropertyControlEnum::cameraDisconnected, this, &ConfigDialog::cameraDisconnected);
        connect(m_frameStartTriggerActivation, &PropertyControlEnum::cameraDisconnected, this, &ConfigDialog::cameraDisconnected);
        connect(m_lineStartTriggerMode, &PropertyControlBool::cameraDisconnected, this, &ConfigDialog::cameraDisconnected);
        connect(m_lineStartTriggerSource, &PropertyControlEnum::cameraDisconnected, this, &ConfigDialog::cameraDisconnected);
        connect(m_lineStartTriggerActivation, &PropertyControlEnum::cameraDisconnected, this, &ConfigDialog::cameraDisconnected);
        connect(m_PWMFrequency, &PropertyControlFloat::cameraDisconnected, this, &ConfigDialog::cameraDisconnected);
        connect(m_acquisitionLineRate, &PropertyControlFloat::cameraDisconnected, this, &ConfigDialog::cameraDisconnected);

        // Update Combobox Entries
        updateFrameStartTriggerActivationItems();
        updateLineStartTriggerActivationItems();
    }
    catch (const std::exception& e)
    {
        qDebug() << "Exception: " << e.what();
        emit messageBoxTrigger("Exception", e.what());
    }
}

void ConfigDialog::toggleFrameStartWidgets(bool triggerEnabled)
{
    // Enable the Trigger Source and Trigger Activation if the FrameStart trigger is enabled
    m_frameStartTriggerSource->setEnabled(triggerEnabled);
    m_frameStartTriggerActivation->setEnabled(triggerEnabled);
}

void ConfigDialog::toggleLineStartWidgets(bool triggerEnabled)
{
    // Disable the Acquisition Line Rate if the LineStart trigger is enabled
    m_acquisitionLineRate->setEnabled(!triggerEnabled);

    // Enable the Trigger Source and Trigger Activation if the LineStart trigger is enabled
    m_lineStartTriggerSource->setEnabled(triggerEnabled);
    m_lineStartTriggerActivation->setEnabled(triggerEnabled);
}

void ConfigDialog::handleTriggerSourceChanged(QString triggerSource)
{
    QObject* senderObject = sender();
    if (senderObject == m_frameStartTriggerSource)
    {
        if (triggerSource == "Software")
        {
            updateFrameStartTriggerActivationItems();
            m_frameStartTriggerActivation->setEnabled(true);
            if (m_lineStartTriggerSource->getValue() != QString("PWM0") || !m_lineStartTriggerMode->getValue())
            {
                m_PWMFrequency->setEnabled(false);
            }
        }
        else if (triggerSource == "PWM0")
        {
            m_frameStartTriggerActivation->setEnabled(false);
            m_PWMFrequency->setEnabled(true);
            try
            {
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("PWMTriggerSource")
                    ->SetCurrentEntry("AcquisitionActive");
            }
            catch (const std::exception& e)
            {
                qDebug() << "Exception: " << e.what();
                emit messageBoxTrigger("Exception", e.what());
            }
        }
        else
        {
            updateFrameStartTriggerActivationItems();
            m_frameStartTriggerActivation->setEnabled(true);
            if (m_lineStartTriggerSource->getValue() != QString("PWM0") || !m_lineStartTriggerMode->getValue())
            {
                m_PWMFrequency->setEnabled(false);
            }
        }
    }
    else if (senderObject == m_lineStartTriggerSource)
    {
        if (triggerSource == "Software")
        {
            updateLineStartTriggerActivationItems();
            m_lineStartTriggerActivation->setEnabled(true);
            if (m_frameStartTriggerSource->getValue() != QString("PWM0") || !m_frameStartTriggerMode->getValue())
            {
                m_PWMGroupBox->setEnabled(false);
            }
        }
        else if (triggerSource == "PWM0")
        {
            m_lineStartTriggerActivation->setEnabled(false);
            m_PWMFrequency->setEnabled(true);
            try
            {
                m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("PWMTriggerSource")
                    ->SetCurrentEntry("AcquisitionActive");
            }
            catch (const std::exception& e)
            {
                qDebug() << "Exception: " << e.what();
                emit messageBoxTrigger("Exception", e.what());
            }
        }
        else
        {
            updateLineStartTriggerActivationItems();
            m_lineStartTriggerActivation->setEnabled(true);
            if (m_frameStartTriggerSource->getValue() != QString("PWM0") || !m_frameStartTriggerMode->getValue())
            {
                m_PWMFrequency->setEnabled(false);
            }
        }
    }
}

void ConfigDialog::handleTriggerModeChanged(bool triggerMode)
{
    QObject* senderObject = sender();

    if (senderObject == m_frameStartTriggerMode)
    {
        // Enable the Trigger Source and Trigger Activation if the FrameStart trigger is enabled
        m_frameStartTriggerSource->setEnabled(triggerMode);
        m_frameStartTriggerActivation->setEnabled(triggerMode);
    }
    else if (senderObject == m_lineStartTriggerMode)
    {
        // Disable the Acquisition Line Rate if the LineStart trigger is enabled
        m_acquisitionLineRate->setEnabled(!triggerMode);

        // Enable the Trigger Source and Trigger Activation if the LineStart trigger is enabled
        m_lineStartTriggerSource->setEnabled(triggerMode);
        m_lineStartTriggerActivation->setEnabled(triggerMode);
    }

    if ((m_frameStartTriggerSource->getValue() == QString("PWM0") && m_frameStartTriggerMode->getValue())
        || (m_lineStartTriggerSource->getValue() == QString("PWM0") && m_lineStartTriggerMode->getValue()))
    {
        m_PWMFrequency->setEnabled(true);
    }
    else
    {
        m_PWMFrequency->setEnabled(false);
    }
}

void ConfigDialog::updateFrameStartTriggerActivationItems()
{
    m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
        ->SetCurrentEntry("FrameStart");

    auto triggerActivations = m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerActivation")->Entries();

    QStringList entryList;
    for (const auto& triggerActivation : triggerActivations)
    {
        if (triggerActivation->AccessStatus() != peak::core::nodes::NodeAccessStatus::NotAvailable
            && triggerActivation->AccessStatus() != peak::core::nodes::NodeAccessStatus::NotImplemented)
        {
            entryList << QString::fromStdString(triggerActivation->StringValue());
        }
    }
    m_frameStartTriggerActivation->setItems(entryList);
}

void ConfigDialog::updateLineStartTriggerActivationItems()
{
    m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
        ->SetCurrentEntry("LineStart");

    auto triggerActivations = m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerActivation")->Entries();

    QStringList entryList;
    for (const auto& triggerActivation : triggerActivations)
    {
        if (triggerActivation->AccessStatus() != peak::core::nodes::NodeAccessStatus::NotAvailable
            && triggerActivation->AccessStatus() != peak::core::nodes::NodeAccessStatus::NotImplemented)
        {
            entryList << QString::fromStdString(triggerActivation->StringValue());
        }
    }
    m_lineStartTriggerActivation->setItems(entryList);
}