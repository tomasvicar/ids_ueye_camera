/*!
 * \file    propertycontrolfloat.cpp
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

#include "propertycontrolfloat.h"

#include <QDebug>

DoubleSlider::DoubleSlider(QWidget* parent)
    : QSlider(parent)
{
    connect(this, &QSlider::valueChanged, this, &DoubleSlider::notifyValueChanged);
}

void DoubleSlider::notifyValueChanged(int value)
{
    double doubleValue = value / 10000.0;
    emit doubleValueChanged(doubleValue);
}

void DoubleSlider::setDoubleValue(double value)
{
    int intValue = static_cast<int>(value * 10000);
    setValue(intValue);
}


PropertyControlFloat::PropertyControlFloat(
    std::string name, std::shared_ptr<peak::core::NodeMap> nodeMap, QObject* parent)
{
    m_sliderMultiplicator = 10000;
    m_nodemapRemoteDevice = nodeMap;
    m_nodeName = name;

    auto min = 0.0;
    auto max = 0.0;

    try
    {
        min = m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>(m_nodeName)->Minimum();
        max = m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>(m_nodeName)->Maximum();
    }
    catch (const std::exception& e)
    {
        qDebug() << "Exception: " << e.what();
        emit messageBoxTrigger("Exception", e.what());
    }

    m_label = new QLabel(this);
    m_label->setText(QString::fromStdString(m_nodeName));

    m_slider = new DoubleSlider(this);
    m_slider->setOrientation(Qt::Horizontal);
    m_slider->setRange(static_cast<int>(min * m_sliderMultiplicator), static_cast<int>(max * m_sliderMultiplicator));
    m_slider->setMinimumWidth(100);

    m_spinBox = new QDoubleSpinBox(this);
    m_spinBox->setRange(min, max);

    try
    {
        auto inc = m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>(m_nodeName)->Increment();
        m_slider->setSingleStep(inc * m_sliderMultiplicator);
        m_spinBox->setSingleStep(inc);
    }
    catch (const std::exception&)
    {
        auto inc = 0.01;
        m_slider->setSingleStep(inc * m_sliderMultiplicator);
        m_spinBox->setSingleStep(inc);
        m_spinBox->setDecimals(4);
    }

    m_layout = new QHBoxLayout();
    m_layout->addWidget(m_label);
    m_layout->addWidget(m_slider);
    m_layout->addWidget(m_spinBox);

    connect(m_slider, &DoubleSlider::doubleValueChanged, this, &PropertyControlFloat::setValue);
    connect(m_spinBox, &QDoubleSpinBox::editingFinished, this, &PropertyControlFloat::onEditingFinished);

    try
    {
        setValue(m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>(m_nodeName)->Value());
    }
    catch (const std::exception& e)
    {
        qDebug() << "Exception: " << e.what();
        emit messageBoxTrigger("Exception", e.what());
    }
}

void PropertyControlFloat::onEditingFinished()
{
    setValue(m_spinBox->value());
}

void PropertyControlFloat::setValue(double value)
{
    if (value < m_spinBox->minimum())
    {
        value = m_spinBox->minimum();
    }
    else if (value > m_spinBox->maximum())
    {
        value = m_spinBox->maximum();
    }

    QObject* senderObj = sender();
    if (senderObj == m_spinBox)
    {
        auto test = m_spinBox->value();
        m_slider->setDoubleValue(value);
    }
    else
    {
        m_spinBox->setValue(value);
    }

    try
    {
        m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>(m_nodeName)->SetValue(value);
        auto newValue = m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>(m_nodeName)->Value();

        m_slider->blockSignals(true);
        m_spinBox->blockSignals(true);

        m_slider->setValue(newValue * m_sliderMultiplicator);
        m_spinBox->setValue(newValue);

        m_slider->blockSignals(false);
        m_spinBox->blockSignals(false);
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

double PropertyControlFloat::getValue()
{
    return m_slider->value() / m_sliderMultiplicator;
}

void PropertyControlFloat::update()
{
    try
    {
        auto min = m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>(m_nodeName)->Minimum();
        auto max = m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>(m_nodeName)->Maximum();
        m_slider->setRange(
            static_cast<int>(min * m_sliderMultiplicator), static_cast<int>(max * m_sliderMultiplicator));
        m_spinBox->setRange(min, max);

        setValue(m_nodemapRemoteDevice->FindNode<peak::core::nodes::FloatNode>(m_nodeName)->Value());
    }
    catch (const std::exception& e)
    {
        qDebug() << "Exception: " << e.what();
        emit messageBoxTrigger("Exception", e.what());
    }
}

void PropertyControlFloat::setEnabled(bool enabled)
{
    m_enabled = enabled;
    m_label->setEnabled(enabled);
    m_slider->setEnabled(enabled);
    m_spinBox->setEnabled(enabled);
}

bool PropertyControlFloat::isEnabled()
{
    return m_enabled;
}

QHBoxLayout* PropertyControlFloat::getLayout()
{
    return m_layout;
}
