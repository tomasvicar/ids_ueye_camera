/*!
 * \file    modelupdatethreadworker.h
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

#ifndef MODELUPDATETHREADWORKER_H
#define MODELUPDATETHREADWORKER_H

#include "nodelistobject.h"

#include <peak/node_map/peak_node_map.hpp>

#include <QMutex>
#include <QObject>
#include <QStringList>

#include <unordered_map>
#include <vector>

class ModelUpdateThreadWorker : public QObject
{
    Q_OBJECT
public:
    enum State
    {
        IDLE,
        BUSY,
        PAUSED,
        CANCELED
    };

    explicit ModelUpdateThreadWorker(QObject* parent = nullptr);
    ~ModelUpdateThreadWorker();

    State currentState();

    void setNodeMap(std::shared_ptr<peak::core::NodeMap> nodeMap);
    void setNodes(std::vector<std::shared_ptr<peak::core::nodes::Node>> nodes);
    void update();
    void pause();
    void resume();

public slots:
    void cancel();
    void nodeChanged(const std::shared_ptr<peak::core::nodes::Node>& changedNode);
    void unregisterAll();

signals:
    void updateList(int minIndex, int maxIndex, QVector<int> changedRoles);
    void updateRequest();
    void started();
    void finished();

private:
    std::shared_ptr<peak::core::NodeMap> m_nodeMap;
    QStringList m_nodeNames;
    std::vector<std::unique_ptr<NodeListObject>> m_objects;
    std::unordered_map<std::string, peak::core::nodes::Node::ChangedCallbackHandle>
        m_nodeChangedCallbackHandles;
    std::vector<std::string> m_changedNodes;

    State m_state = IDLE;
    mutable QMutex mutex;
    mutable QMutex stateMutex;
    void clear();
};

#endif // MODELUPDATETHREADWORKER_H
