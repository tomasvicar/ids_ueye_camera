/*!
 * \file    entrylistobject.cpp
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

#include "entrylistmodel.h"
#include "entrylistobject.h"

#include <QDebug>
#include <QMutexLocker>

EntryListObject::EntryListObject(std::shared_ptr<peak::core::nodes::EnumerationEntryNode> node)
{
    m_node = node;

    try
    {
        m_name = QString(m_node->Name().c_str());
        m_displayName = QString(m_node->DisplayName().c_str());
        m_tooltip = QString(m_node->ToolTip().c_str());
        m_description = QString(m_node->Description().c_str());

        switch (m_node->Visibility())
        {
        case peak::core::nodes::NodeVisibility::Guru:
            m_visibility = "Guru";
            break;
        case peak::core::nodes::NodeVisibility::Expert:
            m_visibility = "Expert";
            break;
        case peak::core::nodes::NodeVisibility::Beginner:
            m_visibility = "Beginner";
            break;
        case peak::core::nodes::NodeVisibility::Invisible:
            m_visibility = "Invisible";
            break;
        }

        switch (m_node->AccessStatus())
        {
        case peak::core::nodes::NodeAccessStatus::ReadOnly:
            m_available = true;
            m_readable = true;
            m_writeable = false;
            m_accessStatus = "ReadOnly";
            break;
        case peak::core::nodes::NodeAccessStatus::ReadWrite:
            m_available = true;
            m_readable = true;
            m_writeable = true;
            m_accessStatus = "ReadWrite";
            break;
        case peak::core::nodes::NodeAccessStatus::WriteOnly:
            m_available = true;
            m_readable = false;
            m_writeable = true;
            m_accessStatus = "WriteOnly";
            break;
        case peak::core::nodes::NodeAccessStatus::NotAvailable:
            m_available = false;
            m_readable = false;
            m_writeable = false;
            m_accessStatus = "NotAvailable";
            break;
        case peak::core::nodes::NodeAccessStatus::NotImplemented:
            m_available = false;
            m_readable = false;
            m_writeable = false;
            m_accessStatus = "NotImplemented";
            break;
        }

        m_numericValue = m_node->NumericValue();
        m_symbolicValue = QString(m_node->SymbolicValue().c_str());
    }
    catch (std::exception& e)
    {
        qDebug() << "[EntryListObject] EXCEPTION: " << e.what();
    }
}

bool EntryListObject::operator==(const EntryListObject& other)
{
    return (m_node == other.m_node && m_name == other.m_name && m_displayName == other.m_displayName
        && m_tooltip == other.m_tooltip && m_description == other.m_description && m_visibility == other.m_visibility
        && m_numericValue == other.m_numericValue && m_symbolicValue == other.m_symbolicValue
        && m_available == other.m_available && m_readable == other.m_readable && m_writeable == other.m_writeable);
}

bool EntryListObject::operator!=(const EntryListObject& other)
{
    return !(this == &other);
}

QVector<int> EntryListObject::updateAll()
{
    QVector<int> list;
    if (updateName())
    {
        list << EntryListModel::NameRole;
    }
    if (updateDisplayName())
    {
        list << EntryListModel::DisplayNameRole;
    }
    if (updateTooltip())
    {
        list << EntryListModel::TooltipRole;
    }
    if (updateDescription())
    {
        list << EntryListModel::DescriptionRole;
    }
    if (updateVisibility())
    {
        list << EntryListModel::VisibilityRole;
    }
    if (updateNumericValue())
    {
        list << EntryListModel::NumericValueRole;
    }
    if (updateSymbolicValue())
    {
        list << EntryListModel::SymbolicValueRole;
    }
    if (updateAccessStatus())
    {
        list << EntryListModel::AvailableRole << EntryListModel::ReadableRole << EntryListModel::WriteableRole
             << EntryListModel::AccessStatusRole;
    }
    return list;
}

QString EntryListObject::name() const
{
    QMutexLocker locker(&mutex);
    return m_name;
}

QString EntryListObject::displayName() const
{
    QMutexLocker locker(&mutex);
    return m_displayName;
}

QString EntryListObject::tooltip() const
{
    QMutexLocker locker(&mutex);
    return m_tooltip;
}

QString EntryListObject::description() const
{
    QMutexLocker locker(&mutex);
    return m_description;
}

QString EntryListObject::visibility() const
{
    QMutexLocker locker(&mutex);
    return m_visibility;
}

long long EntryListObject::numericValue() const
{
    QMutexLocker locker(&mutex);
    return m_numericValue;
}

QString EntryListObject::symbolicValue() const
{
    QMutexLocker locker(&mutex);
    return m_symbolicValue;
}

bool EntryListObject::available() const
{
    QMutexLocker locker(&mutex);
    return m_available;
}

bool EntryListObject::readable() const
{
    QMutexLocker locker(&mutex);
    return m_readable;
}

bool EntryListObject::writeable() const
{
    QMutexLocker locker(&mutex);
    return m_writeable;
}

std::shared_ptr<peak::core::nodes::EnumerationEntryNode> EntryListObject::node() const
{
    QMutexLocker locker(&mutex);
    return m_node;
}

QString EntryListObject::accessStatus() const
{
    QMutexLocker locker(&mutex);
    return m_accessStatus;
}

bool EntryListObject::updateName()
{
    try
    {
        auto newprop = QString(m_node->Name().c_str());
        QMutexLocker locker(&mutex);
        if (newprop != m_name)
        {
            m_name = newprop;
            locker.unlock();
            emit nameChanged();
            return true;
        }
        locker.unlock();
    }
    catch (std::exception& e)
    {
        qDebug() << "[EntryListObject] EXCEPTION: " << e.what();
    }
    return false;
}

bool EntryListObject::updateDisplayName()
{
    try
    {
        auto newprop = QString(m_node->DisplayName().c_str());
        QMutexLocker locker(&mutex);
        if (newprop != m_displayName)
        {
            m_displayName = newprop;
            locker.unlock();
            emit displayNameChanged();
            return true;
        }
        locker.unlock();
    }
    catch (std::exception& e)
    {
        qDebug() << "[EntryListObject] EXCEPTION: " << e.what();
    }
    return false;
}

bool EntryListObject::updateTooltip()
{
    try
    {
        auto newprop = QString(m_node->ToolTip().c_str());
        QMutexLocker locker(&mutex);
        if (newprop != m_tooltip)
        {
            m_tooltip = newprop;
            locker.unlock();
            emit tooltipChanged();
            return true;
        }
        locker.unlock();
    }
    catch (std::exception& e)
    {
        qDebug() << "[EntryListObject] EXCEPTION: " << e.what();
    }
    return false;
}

bool EntryListObject::updateDescription()
{
    try
    {
        auto newprop = QString(m_node->Description().c_str());
        QMutexLocker locker(&mutex);
        if (newprop != m_description)
        {
            m_description = newprop;
            locker.unlock();
            emit descriptionChanged();
            return true;
        }
        locker.unlock();
    }
    catch (std::exception& e)
    {
        qDebug() << "[EntryListObject] EXCEPTION: " << e.what();
    }
    return false;
}

bool EntryListObject::updateVisibility()
{
    QString newprop;
    try
    {
        switch (m_node->Visibility())
        {
        case peak::core::nodes::NodeVisibility::Guru:
            newprop = "Guru";
            break;
        case peak::core::nodes::NodeVisibility::Expert:
            newprop = "Expert";
            break;
        case peak::core::nodes::NodeVisibility::Beginner:
            newprop = "Beginner";
            break;
        case peak::core::nodes::NodeVisibility::Invisible:
            newprop = "Invisible";
            break;
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "[NodeObject::update] EXCEPTION: " << e.what();
        return false;
    }

    QMutexLocker locker(&mutex);
    if (newprop != m_visibility)
    {
        m_visibility = newprop;
        locker.unlock();
        emit visibilityChanged();
        return true;
    }
    locker.unlock();
    return false;
}

bool EntryListObject::updateNumericValue()
{
    try
    {
        auto newprop = m_node->NumericValue();
        QMutexLocker locker(&mutex);
        if (newprop != m_numericValue)
        {
            m_numericValue = newprop;
            locker.unlock();
            emit numericValueChanged();
            return true;
        }
        locker.unlock();
    }
    catch (std::exception& e)
    {
        qDebug() << "[EntryListObject] EXCEPTION: " << e.what();
    }
    return false;
}

bool EntryListObject::updateSymbolicValue()
{
    try
    {
        auto newprop = QString(m_node->SymbolicValue().c_str());
        QMutexLocker locker(&mutex);
        if (newprop != m_symbolicValue)
        {
            m_symbolicValue = newprop;
            locker.unlock();
            emit numericValueChanged();
            return true;
        }
        locker.unlock();
    }
    catch (std::exception& e)
    {
        qDebug() << "[EntryListObject] EXCEPTION: " << e.what();
    }
    return false;
}

bool EntryListObject::updateAccessStatus()
{
    QString newProp;
    bool r;
    bool w;
    bool a;

    try
    {
        switch (m_node->AccessStatus())
        {
        case peak::core::nodes::NodeAccessStatus::ReadWrite:
            newProp = "ReadWrite";
            r = true;
            w = true;
            a = true;
            break;
        case peak::core::nodes::NodeAccessStatus::WriteOnly:
            newProp = "WriteOnly";
            r = false;
            w = true;
            a = true;
            break;
        case peak::core::nodes::NodeAccessStatus::ReadOnly:
            newProp = "ReadOnly";
            r = true;
            w = false;
            a = true;
            break;
        case peak::core::nodes::NodeAccessStatus::NotAvailable:
            newProp = "NotAvailable";
            r = false;
            w = false;
            a = false;
            break;
        case peak::core::nodes::NodeAccessStatus::NotImplemented:
            newProp = "NotImplemented";
            r = false;
            w = false;
            a = false;
            break;
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "[NodeObject::update] EXCEPTION: " << e.what();
        return false;
    }

    QMutexLocker locker(&mutex);
    if (r != m_readable)
    {
        m_readable = r;
        emit readableChanged();
    }
    if (w != m_writeable)
    {
        m_writeable = w;
        emit writeableChanged();
    }
    if (a != m_available)
    {
        m_available = a;
        emit availableChanged();
    }
    if (newProp != m_accessStatus)
    {
        m_accessStatus = newProp;
        locker.unlock();
        emit accessStatusChanged();
        return true;
    }
    locker.unlock();
    return false;
}
