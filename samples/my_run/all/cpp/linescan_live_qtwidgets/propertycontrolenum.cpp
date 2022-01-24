/*!
 * \file    propertycontrolenum.cpp
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

#include "propertycontrolenum.h"

#include <QDebug>

PropertyControlEnum::PropertyControlEnum(std::string name, std::shared_ptr<peak::core::NodeMap> nodeMap,
    QStringList items, QObject* parent, std::string selectorName)
{
    m_nodemapRemoteDevice = nodeMap;
    m_nodeName = name;
    m_selectorName = selectorName;

    m_label = new QLabel(this);
    m_label->setText(QString::fromStdString(m_nodeName));

    m_comboBox = new QComboBox(this);
    connect(m_comboBox, &QComboBox::currentTextChanged, this, &PropertyControlEnum::setValue);
    m_comboBox->addItems(items);

    m_layout = new QHBoxLayout();
    m_layout->addWidget(m_label);
    m_layout->addWidget(m_comboBox);
}

void PropertyControlEnum::setItems(QStringList items)
{
	disconnect(m_comboBox, &QComboBox::currentTextChanged, this, &PropertyControlEnum::setValue);
    m_comboBox->clear();
	connect(m_comboBox, &QComboBox::currentTextChanged, this, &PropertyControlEnum::setValue);
    m_comboBox->addItems(items);
}

void PropertyControlEnum::setValue(QString value)
{
    try
    {
        if (m_selectorName != "" && m_nodeName.substr(0, 7) == "Trigger")
        {
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>("TriggerSelector")
                ->SetCurrentEntry(m_selectorName);
        }

        std::string valueString = value.toUtf8().constData();
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::EnumerationNode>(m_nodeName)
            ->SetCurrentEntry(valueString);
        emit valueChanged(value);
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

QString PropertyControlEnum::getValue()
{
    return m_comboBox->currentText();
}

void PropertyControlEnum::setEnabled(bool enabled)
{
    m_enabled = enabled;
    m_label->setEnabled(enabled);
    m_comboBox->setEnabled(enabled);
}

bool PropertyControlEnum::isEnabled()
{
    return m_enabled;
}

QHBoxLayout* PropertyControlEnum::getLayout()
{
    return m_layout;
}
