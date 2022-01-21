/*!
 * \file    modelupdatethreadworker.cpp
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-02-01
 * \since   1.1.6
 *
 * \brief   The ModelUpdateThreadWorker class is used in a worker thread to
 *          receive callbacks and check for changes of the camera's property
 *          nodes.
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

#include "modelupdatethreadworker.h"

#include <QDebug>
#include <QMutexLocker>
#include <QThread>

#include <chrono>

ModelUpdateThreadWorker::ModelUpdateThreadWorker(QObject* parent)
    : QObject(parent)
{
    connect(this, &ModelUpdateThreadWorker::updateRequest, this, &ModelUpdateThreadWorker::update);
}

void ModelUpdateThreadWorker::clear()
{
    QMutexLocker locker(&mutex);

    // unregister all registered callbacks
    unregisterAll();

    // clear all objects
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

ModelUpdateThreadWorker::~ModelUpdateThreadWorker()
{
    clear();
    emit finished();
}

void ModelUpdateThreadWorker::setNodeMap(std::shared_ptr<peak::core::NodeMap> nodeMap)
{
    m_nodeMap = nodeMap;
}

void ModelUpdateThreadWorker::setNodes(std::vector<std::shared_ptr<peak::core::nodes::Node>> nodes)
{
    clear();

    // add a NodeListObject for each node in 'nodes' and register its callback
    for (const auto& node : nodes)
    {
        try
        {
            m_objects.emplace_back(std::make_unique<NodeListObject>(node));
            m_nodeNames << QString(node->Name().c_str());

            m_nodeChangedCallbackHandles.emplace(node->Name(),
                node->RegisterChangedCallback(
                    [this](const std::shared_ptr<peak::core::nodes::Node>& changedNode) {
                        this->nodeChanged(changedNode);
                    }));
        }
        catch (const std::exception& e)
        {
            qDebug() << "[ModelUpdateThreadWorker::setNodes] EXCEPTION: " << e.what();
        }
    }
}

void ModelUpdateThreadWorker::update()
{
    QMutexLocker stateLocker(&stateMutex);

    // check that worker is idle
    if (m_state != IDLE)
        return;

    QMutexLocker locker(&mutex);

    // check that there are changed nodes that have to be updated
    if (m_changedNodes.empty())
    {
        locker.unlock();
        return;
    }

    m_state = BUSY;
    stateLocker.unlock();
    emit started();

    // swap m_changedNodes to a new list
    auto list = std::vector<std::string>();
    list.swap(m_changedNodes);
    locker.unlock();

    // remove duplicates from the list
    std::sort(list.begin(), list.end());
    list.erase(std::unique(list.begin(), list.end()), list.end());


    // find the index of each node in the list
    QList<int> indices;
    for (auto const& nodeName : list)
    {
        auto index = m_nodeNames.indexOf(nodeName.c_str());

        if (index > -1)
        {
            indices << index;
        }
    }

    // check if the nodes have really been changed
    std::vector<int> changedIndizes;
    QVector<int> changedRoles;

    for (const auto& index : indices)
    {
        stateLocker.relock();
        if (m_state == BUSY)
        {
            stateLocker.unlock();
            auto c = m_objects.at(static_cast<unsigned int>(index))->updateChangeables();
            if (!c.isEmpty())
            {
                // add index of changed object to changed indizes list
                changedIndizes.emplace_back(index);

                // add changed role, if it hast not been added before
                for (const auto& d : c)
                {
                    if (!changedRoles.contains(d))
                    {
                        changedRoles << d;
                    }
                }
            }
        }
    }

    stateLocker.relock();
    if (m_state != BUSY)
    {
        return;
    }
    stateLocker.unlock();

    // emit updateList signal only, if there are really changes
    if (!changedIndizes.empty())
    {
        auto min = *std::min_element(changedIndizes.begin(), changedIndizes.end());
        auto max = *std::max_element(changedIndizes.begin(), changedIndizes.end());
        emit updateList(min, max, changedRoles);
    }

    stateLocker.relock();

    // if worker is still busy, change state to idle
    if (m_state != BUSY)
    {
        return;
    }

    m_state = IDLE;
    stateLocker.unlock();
}


ModelUpdateThreadWorker::State ModelUpdateThreadWorker::currentState()
{
    QMutexLocker stateLocker(&stateMutex);
    return m_state;
}

void ModelUpdateThreadWorker::pause()
{
    QMutexLocker stateLocker(&stateMutex);
    m_state = PAUSED;
    stateLocker.unlock();
}

void ModelUpdateThreadWorker::resume()
{
    QMutexLocker stateLocker(&stateMutex);
    if (m_state == PAUSED)
        m_state = IDLE;
    stateLocker.unlock();
}

void ModelUpdateThreadWorker::cancel()
{
    QMutexLocker stateLocker(&stateMutex);
    if (m_state == BUSY)
    {
        m_state = CANCELED;
    }
    else
    {
        m_state = CANCELED;
        //        finished();
    }
    stateLocker.unlock();
}

void ModelUpdateThreadWorker::nodeChanged(const std::shared_ptr<peak::core::nodes::Node>& changedNode)
{
    QMutexLocker stateLocker(&stateMutex);

    // check if worker has not been canceled
    if (m_state == CANCELED)
    {
        return;
    }
    stateLocker.unlock();

    // add the changed nodes to the list
    auto list = std::vector<std::string>();
    try
    {
        list.emplace_back(changedNode->Name());

        for (const auto& node : changedNode->InvalidatedNodes())
        {
            list.emplace_back(node->Name());
        }
        for (const auto& node : changedNode->SelectedNodes())
        {
            list.emplace_back(node->Name());
        }
        for (const auto& node : changedNode->SelectingNodes())
        {
            list.emplace_back(node->Name());
            for (const auto& subNode : node->SelectedNodes())
            {
                list.emplace_back(subNode->Name());
            }
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "[ModelUpdateThreadWorker::nodeChanged] EXCEPTION: " << e.what();
    }

    // append the new list to the changed nodes list
    QMutexLocker locker(&mutex);
    m_changedNodes.reserve(m_changedNodes.size() + list.size());
    m_changedNodes.insert(m_changedNodes.end(), list.begin(), list.end());
    locker.unlock();

    // No update request if in the meantime the worker was canceled
    if (m_state == CANCELED)
    {
        return;
    }
    emit updateRequest();
}

void ModelUpdateThreadWorker::unregisterAll()
{
    // unregister all registered callbacks
    qDebug() << "[ModelUpdateThreadWorker::unregisterAll]";
    try
    {
        for (const auto& nameHandlePair : m_nodeChangedCallbackHandles)
            m_nodeMap->FindNode(nameHandlePair.first)->UnregisterChangedCallback(nameHandlePair.second);
    }
    catch (const std::exception& e)
    {
        qDebug() << "[ModelUpdateThreadWorker::unregisterAll] EXCEPTION: " << e.what();
    }
}
