/*!
 * \file    PropertyControl_CommandButton.qml
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-03-01
 * \since   1.1.6
 *
 * \brief   Prototype for a property control button,
 *          working with camera property nodes of type 'Command'
 *
 * \version 1.0.0
 *
 * Copyright (C) 2019 - 2021, IDS Imaging Development Systems GmbH.
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

import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.4

Item {
    id: control
    width: 400
    height: contentRow.implicitHeight

    property variant node: dummyNode
    property alias enabled: button.enabled
    property alias nameWidth: nameTxt.width
    property bool autoApply: true
    property bool dirty: false

    signal executed

    onNodeChanged: {
        if (!node)
            node = dummyNode
        if (autoApply) {
            button.clicked.connect(function () {
                console.log("Executing " + node.name)
                node.execute()
                control.executed()
            })
        } else {
            button.clicked.connect(function () {
                control.executed()
            })
        }
    }

    Row {
        id: contentRow
        height: implicitHeight

        Label {
            id: nameTxt
            text: control.node.displayName
            wrapMode: Text.Wrap
            width: 200
            height: implicitHeight
            anchors.verticalCenter: parent.verticalCenter
            color: dirty ? "red" : ""
            ToolTip {
                text: control.node.tooltip + " (" + control.node.type + " "
                      + control.node.accessStatus + ")"
            }
        }

        Button {
            id: button
            property var node: control.node
            text: "Execute"
            style: ButtonStyle {
                label: Label {
                    color: dirty ? "red" : ""
                    text: control.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
            onNodeChanged: {
                button.enabled = Qt.binding(function () {
                    return node.writeable
                })
            }
        }
    }

    Item {
        id: dummyNode
        property string tooltip: ""
        property string description: ""
        property string name: ""
        property string displayName: ""
        property bool readable: false
        property bool available: false
        property bool writeable: false
        function execute() {}
    }
}
