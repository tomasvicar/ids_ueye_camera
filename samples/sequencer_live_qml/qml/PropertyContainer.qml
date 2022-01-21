/*!
 * \file    PropertyContainer.qml
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-03-01
 * \since   1.1.6
 *
 * \brief   Prototype for a container that can hold multiple property controls
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
import QtGraphicalEffects 1.0

Rectangle {
    id: root
    height: column.height
    width: 600
    property alias content: column.children
    property alias title: label.text
    property alias annotation: annotationTxt.text
    property bool applyButtonVisible: false
    property bool resetButtonVisible: false

    signal applyPressed
    signal resetPressed

    layer.enabled: true
    layer.effect: DropShadow {
        transparentBorder: true
        verticalOffset: 3
        radius: 3
        color: "#55000000"
    }

    Label {
        id: label
        anchors.margins: 20
        anchors.left: root.left
        anchors.top: root.top
        visible: text === "" ? false : true
    }

    Column {
        id: column
        width: root.width
        spacing: 20
        padding: 20
        topPadding: 20 + (label.visible ? (label.height + 10) : 0)
        bottomPadding: 20 + Math.max(
                           (annotationTxt.visible ? (annotationTxt.height + 20) : 0),
                           (applyButtonVisible ? (applyButton.height + 20) : 0))
    }

    Text {
        id: annotationTxt
        anchors.margins: 20
        width: root.width - 40 - (applyButtonVisible ? (applyButton.width + 20) : 0)
        anchors.left: root.left
        anchors.bottom: root.bottom
        opacity: 0.5
        font.italic: true
        visible: text === "" ? false : true
        wrapMode: Text.WordWrap
    }

    Button {
        id: applyButton
        anchors.right: parent.right
        anchors.margins: 20
        anchors.bottom: parent.bottom
        visible: applyButtonVisible
        text: "Apply"

        onClicked: {
            applyPressed()
        }
    }

    Button {
        id: resetButton
        anchors.right: applyButton.left
        anchors.margins: 20
        anchors.bottom: parent.bottom
        visible: resetButtonVisible
        text: "Reset"

        onClicked: {
            resetPressed()
        }
    }
}
