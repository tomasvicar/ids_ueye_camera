/*!
 * \file    propertycontrolenum.h
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

#ifndef PROPERTYCONTROLENUM_H
#define PROPERTYCONTROLENUM_H

#include <peak/peak.hpp>

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QWidget>


class PropertyControlEnum : public QWidget
{
    Q_OBJECT

public:
    explicit PropertyControlEnum(std::string name, std::shared_ptr<peak::core::NodeMap> nodeMap,
        QStringList items, QObject* parent = nullptr, std::string selectorName = "");
    void setItems(QStringList items);
    QString getValue();
    void setEnabled(bool enabled);
    bool isEnabled();
    QHBoxLayout* getLayout();

private:
    std::shared_ptr<peak::core::NodeMap> m_nodemapRemoteDevice;
    std::string m_nodeName;
    std::string m_selectorName;

    QHBoxLayout* m_layout = nullptr;
    QLabel* m_label = nullptr;
    QComboBox* m_comboBox = nullptr;
    bool m_enabled;

signals:
    void valueChanged(QString value);
    void messageBoxTrigger(QString messageTitle, QString messageText);
	void cameraDisconnected();

public slots:
    void setValue(QString value);
};

#endif // PROPERTYCONTROLENUM_H
