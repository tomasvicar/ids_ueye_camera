/*!
 * \file    entrylistmodel.cpp
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

#include "entrylistmodel.h"

#include <QDebug>
#include <QMutexLocker>

EntryListModel::EntryListModel()
{}

EntryListModel::~EntryListModel()
{
    for (auto& pointer : m_objects)
    {
        pointer = nullptr;
    }
    m_objects.clear();
}

bool EntryListModel::operator==(const EntryListModel& other)
{
    if (m_node == other.m_node && m_objects.size() == other.m_objects.size()
        && m_entryNames.size() == other.m_entryNames.size()
        && m_entryDisplayNames.size() == other.m_entryDisplayNames.size() && m_currentIndex == other.m_currentIndex)
    {
        for (unsigned int i = 0; i < m_objects.size(); ++i)
        {
            if (m_objects.at(i) != other.m_objects.at(i))
                return false;
        }
        for (int i = 0; i < m_entryNames.size(); ++i)
        {
            if (m_entryNames.at(i) != other.m_entryNames.at(i))
                return false;
        }
        for (int i = 0; i < m_entryDisplayNames.size(); ++i)
        {
            if (m_entryDisplayNames.at(i) != other.m_entryDisplayNames.at(i))
                return false;
        }
    }
    else
    {
        return false;
    }
    return true;
}

bool EntryListModel::operator!=(const EntryListModel& other)
{
    return !(this == &other);
}

int EntryListModel::rowCount(const QModelIndex& index) const
{
    return index.isValid() ? 0 : static_cast<int>(m_objects.size());
}

QVariant EntryListModel::data(const QModelIndex& index, int role) const
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
    case TooltipRole:
        return m_objects.at(uindex)->tooltip();
    case DescriptionRole:
        return m_objects.at(uindex)->description();
    case VisibilityRole:
        return m_objects.at(uindex)->visibility();
    case NumericValueRole:
        return m_objects.at(uindex)->numericValue();
    case SymbolicValueRole:
        return m_objects.at(uindex)->symbolicValue();
    case AvailableRole:
        return m_objects.at(uindex)->available();
    case ReadableRole:
        return m_objects.at(uindex)->readable();
    case WriteableRole:
        return m_objects.at(uindex)->writeable();
    case AccessStatusRole:
        return m_objects.at(uindex)->accessStatus();
    default:
        return QVariant();
    }
}

void EntryListModel::setNode(std::shared_ptr<peak::core::nodes::EnumerationNode> node)
{
    beginResetModel();
    m_objects.clear();
    m_entryNames.clear();
    m_entryDisplayNames.clear();
    m_node = node;

    try
    {
        // create an EntryListObject for all entries of the enumeration node
        for (const auto& entry : node->Entries())
        {
            try
            {
                m_objects.emplace_back(std::make_shared<EntryListObject>(entry));
                m_entryNames << QString(entry->Name().c_str());
                m_entryDisplayNames << QString(entry->DisplayName().c_str());
            }
            catch (std::exception& e)
            {
                qDebug() << "[EntryListModel::setNode] EXCEPTION: " << e.what();
            }
        }
    }
    catch (std::exception& e)
    {
        qDebug() << "[EntryListModel::setNode] EXCEPTION: " << e.what();
    }

    updateCurrentIndex();
    endResetModel();
    emit countChanged();
}

EntryListObject* EntryListModel::get(QString name)
{
    // find the entry with the given name
    auto index = m_entryNames.indexOf(name);

    // if the exact name was not found, try to find an entry that 'contains' the given name
    if (index == -1)
    {
        int i = 0;
        for (const auto& n : m_entryNames)
        {
            if (n.simplified().contains(name.simplified()))
            {
                index = i;
                break;
            }
            ++i;
        }
    }

    QMutexLocker locker(&mutex);
    return m_objects.at(static_cast<unsigned int>(index)).get();
}

EntryListObject* EntryListModel::get(int index)
{
    QMutexLocker locker(&mutex);
    return m_objects.at(static_cast<unsigned int>(index)).get();
}

QStringList EntryListModel::entryDisplayNameList()
{
    QMutexLocker locker(&mutex);
    return m_entryDisplayNames;
}

int EntryListModel::currentIndex()
{
    QMutexLocker locker(&mutex);
    return m_currentIndex;
}

void EntryListModel::setCurrentIndex(int newIndex)
{
    // check if the enumeration node is valid
    if (m_node == nullptr)
    {
        return;
    }

    try
    {
        // check if the enumeration node has write access
        if ((m_node->AccessStatus() == peak::core::nodes::NodeAccessStatus::WriteOnly)
            || (m_node->AccessStatus() == peak::core::nodes::NodeAccessStatus::ReadWrite))
        {
            // update the current index of the enumeration node
            updateCurrentIndex();
            QMutexLocker locker(&mutex);

            // if the new index is not the current index, set the new index as current
            if (newIndex != m_currentIndex)
            {
                // find the entry that belongs the the new index
                auto newEntry = m_objects.at(static_cast<unsigned int>(newIndex));

                // if the entry is availabe, set it as current entry
                if (newEntry->available())
                {
                    m_node->SetCurrentEntry(m_objects.at(static_cast<unsigned int>(newIndex))->node());
                    m_currentIndex = newIndex;
                }
                locker.unlock();
                emit currentIndexChanged();
                return;
            }
            locker.unlock();
        }
    }
    catch (std::exception& e)
    {
        qDebug() << "[EntryListModel::setCurrentIndex] EXCEPTION: " << m_node->Name().c_str() << ": " << e.what();
    }
}

bool EntryListModel::updateAll()
{
    int i = 0;
    std::vector<int> changedIndizes;
    QVector<int> changedRoles;

    // for all entries of the enumeration node, update all properties
    for (const auto& entry : m_objects)
    {
        // update all properties of the entry
        auto c = entry->updateAll();

        // if there are propertyChanges, add entry to the changedIndizes list and add the changed property type to
        // the changedRoles list
        if (!c.isEmpty())
        {
            // add index of changed entry
            changedIndizes.emplace_back(i);

            // add changed role, if it hast not been added before
            for (const auto& d : c)
            {
                if (!changedRoles.contains(d))
                    changedRoles << d;
            }
        }
        ++i;
    }

    // if there have been changes, emit the dataChanged signal
    if (!changedIndizes.empty())
    {
        auto min = *std::min_element(changedIndizes.begin(), changedIndizes.end());
        auto max = *std::max_element(changedIndizes.begin(), changedIndizes.end());
        emit dataChanged(this->index(min, 0), this->index(max, 0), changedRoles);
        return true;
    }
    else
    {
        return false;
    }
}

QHash<int, QByteArray> EntryListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[DisplayNameRole] = "displayName";
    roles[TooltipRole] = "tooltip";
    roles[DescriptionRole] = "description";
    roles[VisibilityRole] = "visibility";
    roles[NumericValueRole] = "numericValue";
    roles[SymbolicValueRole] = "symbolicValue";
    roles[AvailableRole] = "available";
    roles[ReadableRole] = "readable";
    roles[WriteableRole] = "writeable";
    roles[AccessStatusRole] = "accessStatus";
    return roles;
}

void EntryListModel::updateCurrentIndex()
{
    // check if the enumeration node is valid
    if (m_node == nullptr)
    {
        return;
    }

    try
    {
        // check if the enumeration node has read access
        if ((m_node->AccessStatus() == peak::core::nodes::NodeAccessStatus::ReadOnly)
            || (m_node->AccessStatus() == peak::core::nodes::NodeAccessStatus::ReadWrite))
        {
            // get the current entry index of the node
            auto currentEntryName = m_node->CurrentEntry()->Name();
            auto newIndex = m_entryNames.indexOf(currentEntryName.c_str());

            // if the new index is not the current index, set the new index as current
            if (newIndex != m_currentIndex)
            {
                QMutexLocker locker(&mutex);
                m_currentIndex = newIndex;
                locker.unlock();
                emit currentIndexChanged();
            }
        }
    }
    catch (std::exception& e)
    {
        qDebug() << "[EntryListModel::updateCurrentIndex] EXCEPTION: " << e.what();
    }
}
