/*!
 * \file    nodelistobject.cpp
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

#include "nodelistmodel.h"
#include "nodelistobject.h"

#include <peak/node_map/peak_node_map.hpp>

#include <QMutexLocker>

#include <algorithm>

NodeListObject::NodeListObject(std::shared_ptr<peak::core::nodes::Node> node)
{
    m_node = node;
    updateAll();
}

bool NodeListObject::operator==(const NodeListObject& other)
{
    return (m_node == other.m_node && m_name == other.m_name && m_displayName == other.m_displayName
        && m_category == other.m_category && m_type == other.m_type && m_tooltip == other.m_tooltip
        && m_description == other.m_description && m_visibility == other.m_visibility
        && m_entryListModel == other.m_entryListModel && m_value == other.m_value && m_min == other.m_min
        && m_max == other.m_max && m_inc == other.m_inc && m_unit == other.m_unit && m_available == other.m_available
        && m_readable == other.m_readable && m_writeable == other.m_writeable);
}

bool NodeListObject::operator!=(const NodeListObject& other)
{
    return !(this == &other);
}

QVector<int> NodeListObject::updateAll()
{
    // update all properties of the object
    QVector<int> list;
    if (updateName())
    {
        list << NodeListModel::NameRole;
    }
    if (updateDisplayName())
    {
        list << NodeListModel::DisplayNameRole;
    }
    if (updateCategory())
    {
        list << NodeListModel::CategoryRole;
    }
    if (updateType())
    {
        list << NodeListModel::TypeRole;
    }
    if (updateTooltip())
    {
        list << NodeListModel::TooltipRole;
    }
    if (updateDescription())
    {
        list << NodeListModel::DescriptionRole;
    }
    if (updateVisibility())
    {
        list << NodeListModel::VisibilityRole;
    }
    if (updateEntryListModel())
    {
        list << NodeListModel::EntryListModelRole;
    }
    if (updateValue())
    {
        list << NodeListModel::ValueRole;
    }
    if (updateMin())
    {
        list << NodeListModel::MinRole;
    }
    if (updateMax())
    {
        list << NodeListModel::MaxRole;
    }
    if (updateInc())
    {
        list << NodeListModel::IncRole;
    }
    if (updateUnit())
    {
        list << NodeListModel::UnitRole;
    }
    if (updateAccessStatus())
    {
        list << NodeListModel::AvailableRole << NodeListModel::ReadableRole << NodeListModel::WriteableRole
             << NodeListModel::AccessStatusRole;
    }
    return list;
}

QVector<int> NodeListObject::updateChangeables()
{
    // update all properties of the object, that are frequently changed
    QVector<int> list;
    if (updateEntryListModel())
    {
        list << NodeListModel::EntryListModelRole;
    }
    if (updateValue())
    {
        list << NodeListModel::ValueRole;
    }
    if (updateMin())
    {
        list << NodeListModel::MinRole;
    }
    if (updateMax())
    {
        list << NodeListModel::MaxRole;
    }
    if (updateInc())
    {
        list << NodeListModel::IncRole;
    }
    if (updateAccessStatus())
    {
        list << NodeListModel::AvailableRole << NodeListModel::ReadableRole << NodeListModel::WriteableRole
             << NodeListModel::AccessStatusRole;
    }
    return list;
}

void NodeListObject::setCategory(QString categoryName)
{
    QMutexLocker locker(&mutex);
    m_category = categoryName;
    locker.unlock();
}

QString NodeListObject::name() const
{
    QMutexLocker locker(&mutex);
    return m_name;
}

QString NodeListObject::displayName() const
{
    QMutexLocker locker(&mutex);
    return m_displayName;
}

EntryListModel* NodeListObject::entryListModel() const
{
    QMutexLocker locker(&mutex);
    return m_entryListModel.get();
}

QVariant NodeListObject::value() const
{
    QMutexLocker locker(&valueMutex);
    return m_value;
}

QVariant NodeListObject::min() const
{
    QMutexLocker locker(&mutex);
    return m_min;
}

QVariant NodeListObject::max() const
{
    QMutexLocker locker(&mutex);
    return m_max;
}

QVariant NodeListObject::inc() const
{
    QMutexLocker locker(&mutex);
    return m_inc;
}

QString NodeListObject::unit() const
{
    QMutexLocker locker(&mutex);
    return m_unit;
}

bool NodeListObject::available() const
{
    QMutexLocker locker(&accessMutex);
    return m_available;
}

bool NodeListObject::readable() const
{
    QMutexLocker locker(&accessMutex);
    return m_readable;
}

bool NodeListObject::writeable() const
{
    QMutexLocker locker(&accessMutex);
    return m_writeable;
}

QString NodeListObject::accessStatus() const
{
    QMutexLocker locker(&accessMutex);
    return m_accessStatus;
}

QString NodeListObject::category() const
{
    QMutexLocker locker(&mutex);
    return m_category;
}

QString NodeListObject::type() const
{
    QMutexLocker locker(&mutex);
    return m_type;
}

QString NodeListObject::tooltip() const
{
    QMutexLocker locker(&mutex);
    return m_tooltip;
}

QString NodeListObject::description() const
{
    QMutexLocker locker(&mutex);
    return m_description;
}

QString NodeListObject::visibility() const
{
    QMutexLocker locker(&mutex);
    return m_visibility;
}

bool NodeListObject::updateName()
{
    try
    {
        auto newprop = QString(m_node->Name().c_str());
        if (newprop != m_name)
        {
            m_name = newprop;
            emit nameChanged();
            return true;
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "[NodeListObject::update] EXCEPTION: " << e.what();
    }
    return false;
}

bool NodeListObject::updateDisplayName()
{
    try
    {
        auto newprop = QString(m_node->DisplayName().c_str());
        if (newprop != m_displayName)
        {
            m_displayName = newprop;
            emit displayNameChanged();
            return true;
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "[NodeListObject::update] EXCEPTION: " << e.what();
    }
    return false;
}

bool NodeListObject::updateCategory()
{
    return false;
}

bool NodeListObject::updateType()
{
    try
    {
        auto newprop = QString();
        switch (m_node->Type())
        {
        case peak::core::nodes::NodeType::Float:
            newprop = "Float";
            break;
        case peak::core::nodes::NodeType::String:
            newprop = "String";
            break;
        case peak::core::nodes::NodeType::Boolean:
            newprop = "Boolean";
            break;
        case peak::core::nodes::NodeType::Command:
            newprop = "Command";
            break;
        case peak::core::nodes::NodeType::Integer:
            newprop = "Integer";
            break;
        case peak::core::nodes::NodeType::Category:
            newprop = "Category";
            break;
        case peak::core::nodes::NodeType::Register:
            newprop = "Register";
            break;
        case peak::core::nodes::NodeType::Enumeration:
            newprop = "Enumeration";
            break;
        case peak::core::nodes::NodeType::EnumerationEntry:
            newprop = "EnumerationEntry";
            break;
        }
        if (newprop != m_type)
        {
            m_type = newprop;
            emit typeChanged();
            return true;
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "[NodeListObject::update] EXCEPTION: " << e.what();
    }
    return false;
}

bool NodeListObject::updateTooltip()
{
    try
    {
        auto newprop = QString(m_node->ToolTip().c_str());
        if (newprop != m_tooltip)
        {
            m_tooltip = newprop;
            emit tooltipChanged();
            return true;
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "[NodeListObject::update] EXCEPTION: " << e.what();
    }
    return false;
}

bool NodeListObject::updateDescription()
{
    try
    {
        auto newprop = QString(m_node->Description().c_str());
        if (newprop != m_description)
        {
            m_description = newprop;
            emit descriptionChanged();
            return true;
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "[NodeListObject::update] EXCEPTION: " << e.what();
    }
    return false;
}

bool NodeListObject::updateVisibility()
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

    if (newprop != m_visibility)
    {
        m_visibility = newprop;
        emit visibilityChanged();
        return true;
    }
    return false;
}

bool NodeListObject::updateEntryListModel()
{
    try
    {
        if (m_node->Type() == peak::core::nodes::NodeType::Enumeration)
        {
            QMutexLocker locker(&mutex);
            if (m_entryListModel == nullptr)
            {
                m_entryListModel = std::make_shared<EntryListModel>();
                m_entryListModel->setNode(
                    std::dynamic_pointer_cast<peak::core::nodes::EnumerationNode>(m_node));
                locker.unlock();
                emit entryListModelChanged();
                return true;
            }
            else
            {
                if (m_entryListModel->updateAll())
                {
                    locker.unlock();
                    emit entryListModelChanged(); // model changed only, if rowCount changes?
                    return true;
                }
                else
                {
                    locker.unlock();
                    return false;
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "[NodeListObject::update] EXCEPTION: " << e.what();
    }
    return false;
}

bool NodeListObject::updateValue()
{
    auto newprop = QVariant();
    try
    {
        if (m_node->AccessStatus() != peak::core::nodes::NodeAccessStatus::NotAvailable)
        {
            switch (m_node->Type())
            {
            case peak::core::nodes::NodeType::Float:
                newprop = std::dynamic_pointer_cast<peak::core::nodes::FloatNode>(m_node)->Value();
                break;
            case peak::core::nodes::NodeType::String:
                newprop = QString(
                    std::dynamic_pointer_cast<peak::core::nodes::StringNode>(m_node)->Value().c_str());
                break;
            case peak::core::nodes::NodeType::Boolean:
                newprop = std::dynamic_pointer_cast<peak::core::nodes::BooleanNode>(m_node)->Value();
                break;
            case peak::core::nodes::NodeType::Integer:
                newprop = static_cast<qlonglong>(
                    std::dynamic_pointer_cast<peak::core::nodes::IntegerNode>(m_node)->Value());
                break;
            case peak::core::nodes::NodeType::Enumeration:
            {
                QMutexLocker locker(&valueMutex);
                m_entryListModel->updateCurrentIndex();
                newprop = m_entryListModel->currentIndex();
                locker.unlock();
                break;
            }
            case peak::core::nodes::NodeType::Command:
            case peak::core::nodes::NodeType::Category:
            case peak::core::nodes::NodeType::Register:
            case peak::core::nodes::NodeType::EnumerationEntry:
                break;
            }
            QMutexLocker locker(&valueMutex);
            if (newprop != m_value)
            {
                m_value = newprop;
                locker.unlock();
                emit valueChanged();
                return true;
            }
            locker.unlock();
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "[NodeListObject::update] EXCEPTION: " << e.what();
    }
    return false;
}

bool NodeListObject::updateMin()
{
    auto newprop = QVariant();
    try
    {
        if (m_node->AccessStatus() != peak::core::nodes::NodeAccessStatus::NotAvailable)
        {
            switch (m_node->Type())
            {
            case peak::core::nodes::NodeType::Float:
                newprop = std::dynamic_pointer_cast<peak::core::nodes::FloatNode>(m_node)->Minimum();
                break;
            case peak::core::nodes::NodeType::Integer:
                newprop = static_cast<qlonglong>(
                    std::dynamic_pointer_cast<peak::core::nodes::IntegerNode>(m_node)->Minimum());
                break;
            case peak::core::nodes::NodeType::Boolean:
            case peak::core::nodes::NodeType::Enumeration:
            case peak::core::nodes::NodeType::String:
            case peak::core::nodes::NodeType::Command:
            case peak::core::nodes::NodeType::Category:
            case peak::core::nodes::NodeType::Register:
            case peak::core::nodes::NodeType::EnumerationEntry:
                break;
            }
            QMutexLocker locker(&mutex);
            if (newprop != m_min)
            {
                m_min = newprop;
                locker.unlock();
                emit minChanged();
                return true;
            }
            locker.unlock();
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "[NodeListObject::update] EXCEPTION: " << e.what();
    }
    return false;
}

bool NodeListObject::updateMax()
{
    auto newprop = QVariant();
    try
    {
        if (m_node->AccessStatus() != peak::core::nodes::NodeAccessStatus::NotAvailable)
        {
            switch (m_node->Type())
            {
            case peak::core::nodes::NodeType::Float:
                newprop = std::dynamic_pointer_cast<peak::core::nodes::FloatNode>(m_node)->Maximum();
                break;
            case peak::core::nodes::NodeType::Integer:
                newprop = static_cast<qlonglong>(
                    std::dynamic_pointer_cast<peak::core::nodes::IntegerNode>(m_node)->Maximum());
                break;
            case peak::core::nodes::NodeType::Boolean:
            case peak::core::nodes::NodeType::Enumeration:
            case peak::core::nodes::NodeType::String:
            case peak::core::nodes::NodeType::Command:
            case peak::core::nodes::NodeType::Category:
            case peak::core::nodes::NodeType::Register:
            case peak::core::nodes::NodeType::EnumerationEntry:
                break;
            }
            QMutexLocker locker(&mutex);
            if (newprop != m_max)
            {
                m_max = newprop;
                locker.unlock();
                emit maxChanged();
                return true;
            }
            locker.unlock();
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "[NodeListObject::update] EXCEPTION: " << e.what();
    }
    return false;
}

bool NodeListObject::updateInc()
{
    auto newprop = QVariant();
    try
    {
        if (m_node->AccessStatus() != peak::core::nodes::NodeAccessStatus::NotAvailable)
        {
            switch (m_node->Type())
            {
            case peak::core::nodes::NodeType::Float:
                if (std::dynamic_pointer_cast<peak::core::nodes::FloatNode>(m_node)->IncrementType()
                    == peak::core::nodes::NodeIncrementType::FixedIncrement)
                    newprop = std::dynamic_pointer_cast<peak::core::nodes::FloatNode>(m_node)->Increment();
                else
                    newprop = 0.0;
                break;
            case peak::core::nodes::NodeType::Integer:
                if (std::dynamic_pointer_cast<peak::core::nodes::IntegerNode>(m_node)->IncrementType()
                    == peak::core::nodes::NodeIncrementType::FixedIncrement)
                    newprop = static_cast<qlonglong>(
                        std::dynamic_pointer_cast<peak::core::nodes::IntegerNode>(m_node)->Increment());
                else
                    newprop = 0;
                break;
            case peak::core::nodes::NodeType::Boolean:
            case peak::core::nodes::NodeType::Enumeration:
            case peak::core::nodes::NodeType::String:
            case peak::core::nodes::NodeType::Command:
            case peak::core::nodes::NodeType::Category:
            case peak::core::nodes::NodeType::Register:
            case peak::core::nodes::NodeType::EnumerationEntry:
                break;
            }
            QMutexLocker locker(&mutex);
            if (newprop != m_inc)
            {
                m_inc = newprop;
                locker.unlock();
                emit incChanged();
                return true;
            }
            locker.unlock();
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "[NodeListObject::update] EXCEPTION: " << e.what();
    }
    return false;
}

bool NodeListObject::updateUnit()
{
    auto newprop = QString();
    try
    {
        switch (m_node->Type())
        {
        case peak::core::nodes::NodeType::Float:
            newprop = QString(
                std::dynamic_pointer_cast<peak::core::nodes::FloatNode>(m_node)->Unit().c_str());
            break;
        case peak::core::nodes::NodeType::Integer:
            newprop = QString(
                std::dynamic_pointer_cast<peak::core::nodes::IntegerNode>(m_node)->Unit().c_str());
            break;
        case peak::core::nodes::NodeType::Boolean:
        case peak::core::nodes::NodeType::Enumeration:
        case peak::core::nodes::NodeType::String:
        case peak::core::nodes::NodeType::Command:
        case peak::core::nodes::NodeType::Category:
        case peak::core::nodes::NodeType::Register:
        case peak::core::nodes::NodeType::EnumerationEntry:
            break;
        }
        QMutexLocker locker(&mutex);
        if (newprop != m_unit)
        {
            m_unit = newprop;
            locker.unlock();
            emit unitChanged();
            return true;
        }
        locker.unlock();
    }
    catch (const std::exception& e)
    {
        qDebug() << "[NodeListObject::update] EXCEPTION: " << e.what();
    }
    return false;
}

bool NodeListObject::updateAccessStatus()
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

    if (newProp != m_accessStatus)
    {
        bool rc = false;
        bool wc = false;
        bool ac = false;
        QMutexLocker locker(&accessMutex);
        m_accessStatus = newProp;
        if (r != m_readable)
        {
            m_readable = r;
            rc = true;
        }
        if (w != m_writeable)
        {
            m_writeable = w;
            wc = true;
        }
        if (a != m_available)
        {
            m_available = a;
            ac = true;
        }
        locker.unlock();
        emit accessStatusChanged();
        if (rc)
            emit readableChanged();
        if (wc)
            emit writeableChanged();
        if (ac)
            emit availableChanged();
        return true;
    }
    return false;
}

void NodeListObject::execute()
{
    QMutexLocker locker(&mutex);
    try
    {
        auto node = std::dynamic_pointer_cast<peak::core::nodes::CommandNode>(m_node);
        if ((node->AccessStatus() == peak::core::nodes::NodeAccessStatus::ReadWrite)
            || (node->AccessStatus() == peak::core::nodes::NodeAccessStatus::WriteOnly))
        {
            node->Execute();
            node->WaitUntilDone();
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "[NodeListObject::execute] EXCEPTION: " << e.what();
    }
    locker.unlock();
}

void NodeListObject::setValue(QVariant newValue)
{
    try
    {
        if ((m_node->AccessStatus() == peak::core::nodes::NodeAccessStatus::WriteOnly)
            || (m_node->AccessStatus() == peak::core::nodes::NodeAccessStatus::ReadWrite))
        {
            QMutexLocker locker(&valueMutex);
            switch (m_node->Type())
            {
            case peak::core::nodes::NodeType::Float:
            {
                auto node = std::dynamic_pointer_cast<peak::core::nodes::FloatNode>(m_node);
                auto x = std::min(std::max(newValue.toDouble(), node->Minimum()), node->Maximum());
                node->SetValue(x);
                break;
            }
            case peak::core::nodes::NodeType::String:
                std::dynamic_pointer_cast<peak::core::nodes::StringNode>(m_node)
                    ->SetValue(newValue.toString().toStdString());
                break;
            case peak::core::nodes::NodeType::Boolean:
                std::dynamic_pointer_cast<peak::core::nodes::BooleanNode>(m_node)
                    ->SetValue(newValue.toBool());
                break;
            case peak::core::nodes::NodeType::Integer:
            {
                auto node = std::dynamic_pointer_cast<peak::core::nodes::IntegerNode>(m_node);
                auto x = std::min(std::max(newValue.toLongLong(), static_cast<qlonglong>(node->Minimum())),
                    static_cast<qlonglong>(node->Maximum()));
                node->SetValue(x);
                break;
            }
            case peak::core::nodes::NodeType::Enumeration:
            {
                auto node = std::dynamic_pointer_cast<peak::core::nodes::EnumerationNode>(m_node);
                if (newValue.type() == QVariant::String)
                    node->SetCurrentEntry(newValue.toString().toStdString());
                else
                    node->SetCurrentEntry(newValue.toInt());
                break;
            }
            case peak::core::nodes::NodeType::Command:
            case peak::core::nodes::NodeType::Category:
            case peak::core::nodes::NodeType::Register:
            case peak::core::nodes::NodeType::EnumerationEntry:
                break;
            }
            locker.unlock();
            updateValue();
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "[NodeListObject::update] EXCEPTION: " << e.what();
    }
}

void NodeListObject::setValueToMin()
{
    updateMin();
    setValue(this->min());
}

void NodeListObject::setValueToMax()
{
    updateMax();
    setValue(this->max());
}

std::shared_ptr<peak::core::nodes::Node> NodeListObject::node() const
{
    QMutexLocker locker(&mutex);
    return m_node;
}
