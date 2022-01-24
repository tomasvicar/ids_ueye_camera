/*!
 * \file    propertycontrolinteger.cpp
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

#include "propertycontrolinteger.h"

#include <QDebug>

PropertyControlInteger::PropertyControlInteger(
    std::string name, std::shared_ptr<peak::core::NodeMap> nodeMap, QObject* parent)
{
    m_nodemapRemoteDevice = nodeMap;
    m_nodeName = name;

    auto min = 0;
    auto max = 0;

    try
    {
        min = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>(m_nodeName)->Minimum();
        max = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>(m_nodeName)->Maximum();
    }
    catch (const std::exception& e)
    {
        qDebug() << "Exception: " << e.what();
        emit messageBoxTrigger("Exception", e.what());
    }

    m_label = new QLabel(this);
    m_label->setText(QString::fromStdString(m_nodeName));

    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(min, max);
    m_slider->setMinimumWidth(100);

    m_spinBox = new QSpinBox(this);
    m_spinBox->setRange(min, max);

    try
    {
        auto inc = m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>(m_nodeName)->Increment();
        m_slider->setSingleStep(inc);
        m_spinBox->setSingleStep(inc);
    }
    catch (const std::exception&)
    {
        auto inc = 1;
        m_slider->setSingleStep(inc);
        m_spinBox->setSingleStep(inc);
    }

    m_layout = new QHBoxLayout();
    m_layout->addWidget(m_label);
    m_layout->addWidget(m_slider);
    m_layout->addWidget(m_spinBox);

    connect(m_slider, &QSlider::valueChanged, this, &PropertyControlInteger::setValue);
    connect(m_spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
        &PropertyControlInteger::setValue);

    try
    {
        setValue(m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>(m_nodeName)->Value());
    }
    catch (const std::exception& e)
    {
        qDebug() << "Exception: " << e.what();
        emit messageBoxTrigger("Exception", e.what());
    }
}

void PropertyControlInteger::setValue(int value)
{
    if (value % m_slider->singleStep() == 0)
    {
        m_slider->setValue(value);
        m_spinBox->setValue(value);
        try
        {
            m_nodemapRemoteDevice->FindNode<peak::core::nodes::IntegerNode>(m_nodeName)->SetValue(value);
        }
		catch (const peak::core::InternalErrorException& e)
		{
			qDebug() << "InternalErrorException: " << e.what();
			emit cameraDisconnected();
		}
        catch (const std::exception& e)
        {
            qDebug() << "Std-Exception: " << e.what();
            emit messageBoxTrigger("Std-Exception", e.what());
        }
    }
}

int PropertyControlInteger::getValue()
{
    return m_slider->value();
}

void PropertyControlInteger::setEnabled(bool enabled)
{
    m_enabled = enabled;
    m_label->setEnabled(enabled);
    m_slider->setEnabled(enabled);
    m_spinBox->setEnabled(enabled);
}

bool PropertyControlInteger::isEnabled()
{
    return m_enabled;
}

QHBoxLayout* PropertyControlInteger::getLayout()
{
    return m_layout;
}
