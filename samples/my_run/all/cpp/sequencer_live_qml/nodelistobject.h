/*!
 * \file    nodelistobject.h
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \brief   The NodeListObject is a prototype for camera property nodes of
 *          the types 'Enumeration', 'Float', 'Integer', 'Boolean', 'Command'
 *          and 'String'. NodeListObjects can be managed by NodeListModels.
 *
 * \version 1.0.0
 *
 * Copyright (C) 2020 - 2021, IDS Imaging Development Systems GmbH.
 *
 * The information in this document is subject to change without
 * notice and should not be construed as a commitment by IDS Imaging Development Systems GmbH.
 * IDS Imaging Development Systems GmbH does not assume any responsibility
 * for any errors that may appear in this document.
 *
 * This document, or source code, is provided solely as an example
 * of how to utilize IDS Imaging Development Systems GmbH software libraries in a sample application.
 * IDS Imaging Development Systems GmbH does not assume any responsibility
 * for the use or reliability of any portion of this document.
 *
 * General permission to copy or modify is hereby granted.
 */

#ifndef NODELISTOBJECT_H
#define NODELISTOBJECT_H

#include "entrylistmodel.h"

#include <peak/node_map/peak_node_map.hpp>

#include <QMutex>
#include <QObject>

class NodeListObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString displayName READ displayName NOTIFY displayNameChanged)
    Q_PROPERTY(QString category READ category NOTIFY categoryChanged)
    Q_PROPERTY(QString type READ type NOTIFY typeChanged)
    Q_PROPERTY(QString tooltip READ tooltip NOTIFY tooltipChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QString visibility READ visibility NOTIFY visibilityChanged)
    Q_PROPERTY(EntryListModel* entryListModel READ entryListModel NOTIFY entryListModelChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QVariant min READ min NOTIFY minChanged)
    Q_PROPERTY(QVariant max READ max NOTIFY maxChanged)
    Q_PROPERTY(QVariant inc READ inc NOTIFY incChanged)
    Q_PROPERTY(QString unit READ unit NOTIFY unitChanged)
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)
    Q_PROPERTY(bool readable READ readable NOTIFY readableChanged)
    Q_PROPERTY(bool writeable READ writeable NOTIFY writeableChanged)
    Q_PROPERTY(QString accessStatus READ accessStatus NOTIFY accessStatusChanged)

public:
    NodeListObject(std::shared_ptr<peak::core::nodes::Node> node);
    bool operator==(const NodeListObject& other);
    bool operator!=(const NodeListObject& other);

    std::shared_ptr<peak::core::nodes::Node> node() const;
    QString name() const;
    QString displayName() const;
    QString category() const;
    QString type() const;
    QString tooltip() const;
    QString description() const;
    QString visibility() const;
    EntryListModel* entryListModel() const;
    QVariant value() const;
    QVariant min() const;
    QVariant max() const;
    QVariant inc() const;
    QString unit() const;
    bool available() const;
    bool readable() const;
    bool writeable() const;
    QString accessStatus() const;

    void setCategory(QString categoryName);
    void setValue(QVariant newValue);
    void setValueToMin();
    void setValueToMax();

    QVector<int> updateAll();
    QVector<int> updateChangeables();

    bool updateName();
    bool updateDisplayName();
    bool updateCategory();
    bool updateType();
    bool updateTooltip();
    bool updateDescription();
    bool updateVisibility();
    bool updateEntryListModel();
    bool updateValue();
    bool updateMin();
    bool updateMax();
    bool updateInc();
    bool updateUnit();
    bool updateAccessStatus();

public slots:
    void execute();

signals:
    void nameChanged();
    void displayNameChanged();
    void categoryChanged();
    void typeChanged();
    void tooltipChanged();
    void descriptionChanged();
    void visibilityChanged();
    void entryListModelChanged();
    void valueChanged();
    void minChanged();
    void maxChanged();
    void incChanged();
    void unitChanged();
    void availableChanged();
    void readableChanged();
    void writeableChanged();
    void accessStatusChanged();


private:
    std::shared_ptr<peak::core::nodes::Node> m_node;
    QString m_name = "";
    QString m_displayName = "";
    QString m_category = "";
    QString m_type = "";
    QString m_tooltip = "";
    QString m_description = "";
    QString m_visibility = "";
    std::shared_ptr<EntryListModel> m_entryListModel = nullptr;
    QVariant m_value = 0;
    QVariant m_min = 0;
    QVariant m_max = 0;
    QVariant m_inc = 0;
    QString m_unit = "";
    bool m_available = false;
    bool m_readable = false;
    bool m_writeable = false;
    QString m_accessStatus = "NotAvailable";

    mutable QMutex mutex;
    mutable QMutex valueMutex;
    mutable QMutex accessMutex;
};


#endif // NODELISTOBJECT_H
