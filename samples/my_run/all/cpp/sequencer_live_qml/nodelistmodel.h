/*!
 * \file    nodelistmodel.h
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \brief   The NodeListModel is a prototype for a camera's node map that can
 *          be displayed automatically in QML.
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

#ifndef NODELISTMODEL_H
#define NODELISTMODEL_H

#include "modelupdatethreadworker.h"
#include "nodelistobject.h"

#include <peak/node_map/peak_node_map.hpp>

#include <QAbstractListModel>
#include <QDebug>
#include <QMutex>
#include <QStringList>
#include <QThread>

#include <vector>

class NodeListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles
    {
        NameRole = Qt::UserRole + 1,
        DisplayNameRole,
        CategoryRole,
        TypeRole,
        TooltipRole,
        DescriptionRole,
        VisibilityRole,
        EntryListModelRole,
        ValueRole,
        MinRole,
        MaxRole,
        IncRole,
        UnitRole,
        AvailableRole,
        ReadableRole,
        WriteableRole,
        AccessStatusRole,
        NodeObjectRole
    };

    NodeListModel();
    ~NodeListModel() override;
    void setNodeMap(std::shared_ptr<peak::core::NodeMap> nodeMap);
    int rowCount(const QModelIndex& index = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE NodeListObject* get(QString name);
    Q_INVOKABLE NodeListObject* get(int index);
    void pauseUpdating();
    void resumeUpdating();
    void stopUpdating();
    QThread* updateThread();

public slots:
    void updateList(int a, int b, QVector<int> changedRoles = QVector<int>());

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    std::vector<std::shared_ptr<NodeListObject>> m_objects;
    std::shared_ptr<peak::core::NodeMap> m_nodeMap;
    QStringList m_nodeNames;

    ModelUpdateThreadWorker* m_updateThreadWorker = nullptr;
    QThread* m_updateThread = nullptr;
    int m_objectCount;
    void clear();

    mutable QMutex mutex;
};

#endif // NODELISTMODEL_H
