/*!
 * \file    configdialog.h
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

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include "propertycontrolbool.h"
#include "propertycontrolcommand.h"
#include "propertycontrolenum.h"
#include "propertycontrolfloat.h"
#include "propertycontrolinteger.h"

#include <peak/peak.hpp>

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

class BackEnd;

class ConfigDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigDialog(std::shared_ptr<peak::core::NodeMap> nodeMap, BackEnd* parent = nullptr);

private:
    BackEnd* m_parent = nullptr;
    QDialogButtonBox* m_dialogButtons = nullptr;
    QVBoxLayout* m_vLayout = nullptr;

    std::shared_ptr<peak::core::NodeMap> m_nodemapRemoteDevice;

    PropertyControlInteger* m_offsetY = nullptr;
    PropertyControlInteger* m_height = nullptr;

    PropertyControlBool* m_frameStartTriggerMode = nullptr;
    PropertyControlEnum* m_frameStartTriggerSource = nullptr;
    PropertyControlEnum* m_frameStartTriggerActivation = nullptr;

    PropertyControlBool* m_lineStartTriggerMode = nullptr;
    PropertyControlEnum* m_lineStartTriggerSource = nullptr;
    PropertyControlEnum* m_lineStartTriggerActivation = nullptr;

    PropertyControlFloat* m_acquisitionLineRate = nullptr;
    PropertyControlFloat* m_PWMFrequency = nullptr;

    QGroupBox* m_offsetGroupBox = nullptr;
    QGroupBox* m_heightGroupBox = nullptr;
    QGroupBox* m_frameStartGroupBox = nullptr;
    QGroupBox* m_lineStartGroupBox = nullptr;
    QGroupBox* m_PWMGroupBox = nullptr;

    void createPropertyControls();
	void updateFrameStartTriggerActivationItems();
    void updateLineStartTriggerActivationItems();

signals:
    void configSubmitted(int offsetY, int imageHeight, std::pair<bool, std::pair<QString, QString>> frameStartTrigger,
        std::pair<bool, std::pair<QString, QString>> lineStartTrigger, int acquisitionLineRate, int PWMFrequency);
    void dialogRejected();
    void messageBoxTrigger(QString messageTitle, QString messageText);
	void cameraDisconnected();

private slots:
    void applyConfig();
    void toggleFrameStartWidgets(bool triggerEnabled);
    void toggleLineStartWidgets(bool triggerEnabled);
    void handleTriggerSourceChanged(QString triggerSource);
    void handleTriggerModeChanged(bool triggerMode);
};

#endif // CONFIGDIALOG_H
