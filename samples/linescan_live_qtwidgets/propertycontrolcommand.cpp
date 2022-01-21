/*!
 * \file    propertycontrolcommand.cpp
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

#include "propertycontrolcommand.h"

#include <QDebug>

PropertyControlCommand::PropertyControlCommand(
    std::string name, std::shared_ptr<peak::core::NodeMap> nodeMap, QObject* parent)
{
    m_nodemapRemoteDevice = nodeMap;
    m_nodeName = name;

    m_label = new QLabel(this);
    m_label->setText(QString::fromStdString(m_nodeName));

    m_button = new QPushButton(this);
    m_button->setText("Execute");

    m_layout = new QHBoxLayout();
    m_layout->addWidget(m_label);
    m_layout->addWidget(m_button);

    connect(m_button, &QPushButton::pressed, this, &PropertyControlCommand::buttonPressed);
    connect(m_button, &QPushButton::pressed, this, &PropertyControlCommand::execute);
}

void PropertyControlCommand::execute()
{
    try
    {
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>(m_nodeName)->Execute();
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::CommandNode>(m_nodeName)->WaitUntilDone();
    }
	catch (const peak::core::InternalErrorException& e)
	{
		qDebug() << "InternalErrorException: " << e.what();
		emit cameraDisconnected();
	}
    catch (const std::exception& e)
    {
        qDebug() << "Exception: " << e.what();
        emit messageBoxTrigger("Exception", e.what());
    }
}

void PropertyControlCommand::setEnabled(bool enabled)
{
    m_enabled = enabled;
    m_label->setEnabled(enabled);
    m_button->setEnabled(enabled);
}

bool PropertyControlCommand::isEnabled()
{
    return m_enabled;
}

QHBoxLayout* PropertyControlCommand::getLayout()
{
    return m_layout;
}
