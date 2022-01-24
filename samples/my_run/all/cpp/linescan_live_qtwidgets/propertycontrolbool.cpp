/*!
 * \file    propertycontrolbool.cpp
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

#include "propertycontrolbool.h"

#include <QDebug>

PropertyControlBool::PropertyControlBool(
    std::string name, std::shared_ptr<peak::core::NodeMap> nodeMap, QObject* parent, std::string selectorName)
{
    m_nodemapRemoteDevice = nodeMap;
    m_nodeName = name;
    m_selectorName = selectorName;

    m_label = new QLabel(this);
    m_label->setText(QString::fromStdString(m_nodeName));

    m_checkBox = new QCheckBox(this);
    m_checkBox->setText("On");

    m_layout = new QHBoxLayout();
    m_layout->addWidget(m_label);
    m_layout->addWidget(m_checkBox);

    connect(m_checkBox, &QCheckBox::toggled, this, &PropertyControlBool::valueChanged);
    connect(m_checkBox, &QCheckBox::toggled, this, &PropertyControlBool::setValue);
}

void PropertyControlBool::setValue(bool value)
{
    try
    {
        if (m_selectorName != "" && m_nodeName.rfind("Trigger", 0) == 0)
        {
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                ->SetCurrentEntry(m_selectorName);
        }

        m_checkBox->setChecked(value);
        if (value)
        {
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>(m_nodeName)
                ->SetCurrentEntry("On");
        }
        else
        {
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>(m_nodeName)
                ->SetCurrentEntry("Off");
        }
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

bool PropertyControlBool::getValue()
{
    return m_checkBox->isChecked();
}

void PropertyControlBool::setEnabled(bool enabled)
{
    m_enabled = enabled;
    m_label->setEnabled(enabled);
    m_checkBox->setEnabled(enabled);
}

bool PropertyControlBool::isEnabled()
{
    return m_enabled;
}

QHBoxLayout* PropertyControlBool::getLayout()
{
    return m_layout;
}
