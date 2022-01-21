/*!
 * \file    entrylistmodel.h
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \brief   The EntryListModel is a prototype for camera property nodes of
 *          type "Enumeration" to hold the different enumeration entries.
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

#ifndef ENTRYLISTMODEL_H
#define ENTRYLISTMODEL_H

#include "entrylistobject.h"

#include <peak/node_map/peak_node_map.hpp>

#include <QAbstractListModel>
#include <QDebug>
#include <QMutex>
#include <QObject>

#include <vector>

class EntryListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    enum Roles
    {
        NameRole = Qt::UserRole + 1,
        DisplayNameRole,
        TooltipRole,
        DescriptionRole,
        VisibilityRole,
        NumericValueRole,
        SymbolicValueRole,
        AvailableRole,
        ReadableRole,
        WriteableRole,
        AccessStatusRole
    };

    EntryListModel();
    ~EntryListModel() override;
    bool operator==(const EntryListModel& other);
    bool operator!=(const EntryListModel& other);

    int rowCount(const QModelIndex& index = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    void setNode(std::shared_ptr<peak::core::nodes::EnumerationNode> node);
    Q_INVOKABLE EntryListObject* get(QString name);
    Q_INVOKABLE EntryListObject* get(int index);
    Q_INVOKABLE QStringList entryDisplayNameList();
    int currentIndex();
    void setCurrentIndex(int newIndex);
    bool updateAll();
    void updateCurrentIndex();

signals:
    void currentIndexChanged();
    void countChanged();

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    std::shared_ptr<peak::core::nodes::EnumerationNode> m_node;
    std::vector<std::shared_ptr<EntryListObject>> m_objects = std::vector<std::shared_ptr<EntryListObject>>();
    QStringList m_entryNames = QStringList();
    QStringList m_entryDisplayNames = QStringList();
    int m_currentIndex = 0;
    mutable QMutex mutex;
};

#endif // ENTRYLISTMODEL_H
