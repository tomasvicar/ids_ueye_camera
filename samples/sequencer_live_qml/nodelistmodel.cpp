/*!
 * \file    nodelistmodel.cpp
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

#include "nodelistmodel.h"

#include <QDebug>
#include <QMutexLocker>

NodeListModel::NodeListModel()
{
    m_updateThread = new QThread;
    m_updateThreadWorker = new ModelUpdateThreadWorker();
    m_updateThreadWorker->moveToThread(m_updateThread);

    connect(m_updateThreadWorker, &ModelUpdateThreadWorker::destroyed, m_updateThread, &QThread::quit);
    connect(m_updateThreadWorker, &ModelUpdateThreadWorker::updateList, this, &NodeListModel::updateList);

    m_updateThread->start();
}

NodeListModel::~NodeListModel()
{
    clear();
}

void NodeListModel::clear()
{
    m_objectCount = 0;
    if (!m_objects.empty())
    {
        for (auto& pointer : m_objects)
        {
            pointer = nullptr;
        }
        m_objects.clear();
    }
    m_nodeNames.clear();
}

void NodeListModel::setNodeMap(std::shared_ptr<peak::core::NodeMap> nodeMap)
{
    beginResetModel();
    clear();

    std::vector<std::shared_ptr<peak::core::nodes::Node>> nodes;
    m_nodeMap = nodeMap;

    // find the 'Root' node of the nodeMap
    auto rootNode = nodeMap->FindNode<peak::core::nodes::CategoryNode>("Root");

    // iterate through all category subnodes of 'Root'
    for (const auto& c : rootNode->SubNodes())
    {
        auto category = std::dynamic_pointer_cast<peak::core::nodes::CategoryNode>(c);

        // iterate through all nodes of each category
        for (const auto& node : category->SubNodes())
        {
            // skip chunk nodes
            if (QString(node->Name().c_str()).contains("Chunk"))
                break;

            // add a NodeListObject for each node that is visible and implemented
            if ((node->AccessStatus() != peak::core::nodes::NodeAccessStatus::NotImplemented)
                && (node->Visibility() != peak::core::nodes::NodeVisibility::Invisible))
            {
                m_objects.emplace_back(std::make_shared<NodeListObject>(node));
                m_objects.at(m_objects.size() - 1)->setCategory(QString(category->DisplayName().c_str()));
                m_nodeNames << QString(node->Name().c_str());

                nodes.push_back(node);
            }
        }
    }

    // configure the ModelUpdateThreadWorker
    m_updateThreadWorker->setNodeMap(m_nodeMap);
    m_updateThreadWorker->setNodes(nodes);

    endResetModel();
}

int NodeListModel::rowCount(const QModelIndex& index) const
{
    return index.isValid() ? 0 : static_cast<int>(m_objects.size()); // m_objectCount;
}

QVariant NodeListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();
    auto uindex = static_cast<unsigned int>(index.row());

    switch (role)
    {
    case NameRole:
        return m_objects.at(uindex)->name();
    case DisplayNameRole:
        return m_objects.at(uindex)->displayName();
    case CategoryRole:
        return m_objects.at(uindex)->category();
    case TypeRole:
        return m_objects.at(uindex)->type();
    case TooltipRole:
        return m_objects.at(uindex)->tooltip();
    case DescriptionRole:
        return m_objects.at(uindex)->description();
    case VisibilityRole:
        return m_objects.at(uindex)->visibility();
    case EntryListModelRole:
        return QVariant::fromValue<EntryListModel*>(m_objects.at(uindex)->entryListModel());
    case ValueRole:
        return m_objects.at(uindex)->value();
    case MinRole:
        return m_objects.at(uindex)->min();
    case MaxRole:
        return m_objects.at(uindex)->max();
    case IncRole:
        return m_objects.at(uindex)->inc();
    case UnitRole:
        return m_objects.at(uindex)->unit();
    case AvailableRole:
        return m_objects.at(uindex)->available();
    case ReadableRole:
        return m_objects.at(uindex)->readable();
    case WriteableRole:
        return m_objects.at(uindex)->writeable();
    case AccessStatusRole:
        return m_objects.at(uindex)->accessStatus();
    case NodeObjectRole:
        return QVariant::fromValue<NodeListObject*>(m_objects.at(uindex).get());
    default:
        return QVariant();
    }
}

Q_INVOKABLE NodeListObject* NodeListModel::get(QString name)
{
    QMutexLocker locker(&mutex);
    auto index = m_nodeNames.indexOf(name);
    if (index > -1)
    {
        return m_objects.at(static_cast<unsigned int>(index)).get();
    }
    else
    {
        qDebug() << "[NodeListModel::get] ERROR: Invalid node name";
        return nullptr;
    }
}

Q_INVOKABLE NodeListObject* NodeListModel::get(int index)
{
    QMutexLocker locker(&mutex);
    auto uindex = static_cast<unsigned int>(index);
    if (uindex < m_objects.size())
    {
        return m_objects.at(uindex).get();
    }
    else
    {
        qDebug() << "[NodeListModel::get] ERROR: Invalid Index";
        return nullptr;
    }
}

void NodeListModel::pauseUpdating()
{
    m_updateThreadWorker->pause();
}

void NodeListModel::resumeUpdating()
{
    m_updateThreadWorker->resume();
}

QThread* NodeListModel::updateThread()
{
    return m_updateThread;
}

void NodeListModel::stopUpdating()
{
    m_updateThreadWorker->cancel();
    delete m_updateThreadWorker;
    m_updateThreadWorker = nullptr;
    m_updateThread->wait();
}

void NodeListModel::updateList(int a, int b, QVector<int> changedRoles)
{
    QMutexLocker locker(&mutex);

    // if changedRoles is empty, update all properties of the objects from index a to index b
    // else update only the properties that are contained in changedRoles
    if (changedRoles.isEmpty())
    {
        for (int index = a; index <= b; ++index)
        {
            m_objects.at(static_cast<unsigned int>(index))->updateAll();
        }
    }
    else
    {
        for (int index = a; index <= b; ++index)
        {
            for (const auto& changedRole : changedRoles)
            {
                switch (changedRole)
                {
                case NameRole:
                {
                    m_objects.at(static_cast<unsigned int>(index))->updateName();
                    break;
                }
                case DisplayNameRole:
                {
                    m_objects.at(static_cast<unsigned int>(index))->updateDisplayName();
                    break;
                }
                case CategoryRole:
                {
                    m_objects.at(static_cast<unsigned int>(index))->updateCategory();
                    break;
                }
                case TypeRole:
                {
                    m_objects.at(static_cast<unsigned int>(index))->updateType();
                    break;
                }
                case TooltipRole:
                {
                    m_objects.at(static_cast<unsigned int>(index))->updateTooltip();
                    break;
                }
                case DescriptionRole:
                {
                    m_objects.at(static_cast<unsigned int>(index))->updateDescription();
                    break;
                }
                case VisibilityRole:
                {
                    m_objects.at(static_cast<unsigned int>(index))->updateVisibility();
                    break;
                }
                case EntryListModelRole:
                {
                    m_objects.at(static_cast<unsigned int>(index))->updateEntryListModel();
                    break;
                }
                case ValueRole:
                {
                    m_objects.at(static_cast<unsigned int>(index))->updateValue();
                    break;
                }
                case MinRole:
                {
                    m_objects.at(static_cast<unsigned int>(index))->updateMin();
                    break;
                }
                case MaxRole:
                {
                    m_objects.at(static_cast<unsigned int>(index))->updateMax();
                    break;
                }
                case IncRole:
                {
                    m_objects.at(static_cast<unsigned int>(index))->updateInc();
                    break;
                }
                case UnitRole:
                {
                    m_objects.at(static_cast<unsigned int>(index))->updateUnit();
                    break;
                }
                case AccessStatusRole:
                {
                    m_objects.at(static_cast<unsigned int>(index))->updateAccessStatus();
                    break;
                }
                }
            }
        }

        // remove EntryListModelRole from changed indices
        auto entryIndex = changedRoles.indexOf(EntryListModelRole);
        if (entryIndex > -1)
        {
            changedRoles.remove(entryIndex);
        }
    }
    locker.unlock();

    // emit the dataChanged signal
    emit dataChanged(this->index(a, 0), this->index(b, 0), changedRoles);
}

QHash<int, QByteArray> NodeListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[DisplayNameRole] = "displayName";
    roles[CategoryRole] = "category";
    roles[TypeRole] = "type";
    roles[TooltipRole] = "tooltip";
    roles[DescriptionRole] = "description";
    roles[VisibilityRole] = "visibility";
    roles[EntryListModelRole] = "entryListModel";
    roles[ValueRole] = "value";
    roles[MinRole] = "min";
    roles[MaxRole] = "max";
    roles[IncRole] = "inc";
    roles[UnitRole] = "unit";
    roles[AvailableRole] = "available";
    roles[ReadableRole] = "readable";
    roles[WriteableRole] = "writeable";
    roles[AccessStatusRole] = "accessStatus";
    roles[NodeObjectRole] = "nodeObject";
    return roles;
}
