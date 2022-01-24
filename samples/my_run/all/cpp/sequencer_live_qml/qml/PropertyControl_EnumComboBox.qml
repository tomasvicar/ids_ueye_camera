/*!
 * \file    PropertyControl_EnumComboBox.qml
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-03-01
 * \since   1.1.6
 *
 * \brief   Prototype for a property control combo box,
 *          working with camera property nodes of type 'Enumeration'
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

Item {
    id: control
    width: 400
    height: contentRow.height

    property variant node: dummyNode
    property alias currentIndex: comboBox.currentIndex
    property alias currentText: comboBox.currentText
    property alias nameWidth: nameTxt.width
    property int modelCount: {
        return node.entryListModel.rowCount()
    }
    property bool autoApply: true
    property bool autoUpdate: true
    property alias dummyNode: dummyNode

    onNodeChanged: {
        if (!node)
            control.node = Qt.binding(function () {
                return dummyNode
            })

        if (autoUpdate) {
            comboBox.model = Qt.binding(function () {
                return node.entryListModel
            })
            node.entryListModel.currentIndexChanged.connect(update)
        } else {
            comboBox.model = node.entryListModel
            update()
        }

        if (autoApply) {
            comboBox.visible = Qt.binding(function () {
                return node.writeable
            })
            text.visible = Qt.binding(function () {
                return comboBox.visible ? false : true
            })
            text.available = Qt.binding(function () {
                return node.available
            })
            comboBox.onCurrentIndexChanged.connect(apply)
        } else {
            comboBox.visible = true
            text.visible = false
        }
    }

    function apply() {
        node.entryListModel.currentIndex = comboBox.currentIndex
    }
    function update() {
        comboBox.currentIndex = node.entryListModel.currentIndex
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
            ToolTip {
                text: control.node.tooltip + " (" + control.node.type + " "
                      + control.node.accessStatus + ")"
            }
        }

        ComboBox {
            id: comboBox
            width: control.width - control.nameWidth
            property var node: control.node
            currentIndex: 0
            textRole: "displayName"
            model: dummyModel
        }

        Label {
            id: text
            width: parent.width - nameTxt.width - parent.spacing
            height: implicitHeight
            anchors.verticalCenter: parent.verticalCenter
            property bool available: true
            text: available ? comboBox.currentText : "not available"
            opacity: 0.5
            font.italic: available ? false : true
            visible: false
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
        property alias entryListModel: entryListModel
        ListModel {
            id: entryListModel
            property int currentIndex: 0
            ListElement {
                name: ""
                displayName: ""
                readable: false
                available: true
            }
            function rowCount() {
                return 1
            }
        }
    }
}
