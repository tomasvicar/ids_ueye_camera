/*!
 * \file    propertycontrolcommand.h
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

#ifndef PROPERTYCONTROLCOMMAND_H
#define PROPERTYCONTROLCOMMAND_H

#include <peak/peak.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QWidget>


class PropertyControlCommand : public QWidget
{
    Q_OBJECT

public:
    explicit PropertyControlCommand(
        std::string name, std::shared_ptr<peak::core::NodeMap> nodeMap, QObject* parent = nullptr);
    bool isEnabled();
    QHBoxLayout* getLayout();


private:
    std::shared_ptr<peak::core::NodeMap> m_nodemapRemoteDevice;
    std::string m_nodeName;

    QHBoxLayout* m_layout = nullptr;
    QLabel* m_label = nullptr;
    QPushButton* m_button = nullptr;
    bool m_enabled;

signals:
    void buttonPressed();
    void messageBoxTrigger(QString messageTitle, QString messageText);
	void cameraDisconnected();

public slots:
    void setEnabled(bool enabled);
    void execute();
};

#endif // PROPERTYCONTROLCOMMAND_H
