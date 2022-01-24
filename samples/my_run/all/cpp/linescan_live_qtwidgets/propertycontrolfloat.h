/*!
 * \file    propertycontrolfloat.h
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

#ifndef PROPERTYCONTROLFLOAT_H
#define PROPERTYCONTROLFLOAT_H

#include <peak/peak.hpp>

#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QObject>
#include <QSlider>
#include <QString>
#include <QWidget>

class DoubleSlider : public QSlider
{
    Q_OBJECT

public:
    DoubleSlider(QWidget* parent = nullptr);

signals:
    void doubleValueChanged(double value);

public slots:
    void notifyValueChanged(int value);
    void setDoubleValue(double value);
};

class PropertyControlFloat : public QWidget
{
    Q_OBJECT

public:
    explicit PropertyControlFloat(
        std::string name, std::shared_ptr<peak::core::NodeMap> nodeMap, QObject* parent = nullptr);
    double getValue();
    void setEnabled(bool enabled);
    bool isEnabled();
    QHBoxLayout* getLayout();


private:
    std::shared_ptr<peak::core::NodeMap> m_nodemapRemoteDevice;
    std::string m_nodeName;

    QHBoxLayout* m_layout = nullptr;
    QLabel* m_label = nullptr;
    DoubleSlider* m_slider = nullptr;
    QDoubleSpinBox* m_spinBox = nullptr;
    bool m_enabled;
    int m_sliderMultiplicator = 10000;

signals:
    void valueChanged(double value);
    void messageBoxTrigger(QString messageTitle, QString messageText);
	void cameraDisconnected();

public slots:
    void onEditingFinished();
    void setValue(double value);
    void update();
};

#endif // PROPERTYCONTROLFLOAT_H
