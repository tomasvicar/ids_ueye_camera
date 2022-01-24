/*!
 * \file    entrylistobject.h
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \brief   The EntryListObject is a prototype for camera property nodes of
 *          type "EnumerationEntry". EntryListObjects can be managed by
 *          EntryListModels.
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

#ifndef ENTRYLISTOBJECT_H
#define ENTRYLISTOBJECT_H

#include <peak/node_map/peak_node_map.hpp>

#include <QMutex>
#include <QObject>

class EntryListObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString displayName READ displayName NOTIFY displayNameChanged)
    Q_PROPERTY(QString tooltip READ tooltip NOTIFY tooltipChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QString visibility READ visibility NOTIFY visibilityChanged)
    Q_PROPERTY(long long numericValue READ numericValue NOTIFY numericValueChanged)
    Q_PROPERTY(QString symbolicValue READ symbolicValue NOTIFY symbolicValueChanged)
    Q_PROPERTY(bool available READ available NOTIFY availableChanged)
    Q_PROPERTY(bool readable READ readable NOTIFY readableChanged)
    Q_PROPERTY(bool writeable READ writeable NOTIFY writeableChanged)
    Q_PROPERTY(QString accessStatus READ accessStatus NOTIFY accessStatusChanged)

public:
    EntryListObject(std::shared_ptr<peak::core::nodes::EnumerationEntryNode>);
    bool operator==(const EntryListObject& other);
    bool operator!=(const EntryListObject& other);

    QVector<int> updateAll();

    QString name() const;
    QString displayName() const;
    QString tooltip() const;
    QString description() const;
    QString visibility() const;
    long long numericValue() const;
    QString symbolicValue() const;
    bool available() const;
    bool readable() const;
    bool writeable() const;
    QString accessStatus() const;

    std::shared_ptr<peak::core::nodes::EnumerationEntryNode> node() const;

signals:
    void nameChanged();
    void displayNameChanged();
    void tooltipChanged();
    void descriptionChanged();
    void visibilityChanged();
    void numericValueChanged();
    void symbolicValueChanged();
    void availableChanged();
    void readableChanged();
    void writeableChanged();
    void accessStatusChanged();

private:
    std::shared_ptr<peak::core::nodes::EnumerationEntryNode> m_node;
    QString m_name;
    QString m_displayName;
    QString m_tooltip;
    QString m_description;
    QString m_visibility;
    long long m_numericValue;
    QString m_symbolicValue;
    bool m_available;
    bool m_readable;
    bool m_writeable;
    QString m_accessStatus;

    bool updateName();
    bool updateDisplayName();
    bool updateTooltip();
    bool updateDescription();
    bool updateVisibility();
    bool updateNumericValue();
    bool updateSymbolicValue();
    bool updateAccessStatus();

    mutable QMutex mutex;
};
#endif // ENTRYLISTOBJECT_H
