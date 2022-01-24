/*!
 * \file    ToolTip.qml
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-03-01
 * \since   1.1.6
 *
 * \brief   Prototype for showing a tooltip on an element
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

import QtQuick 2.0

MouseArea {
    property alias tip: tooltip
    property alias text: textContainer.text
    property alias textOffset: tooltip.x
    property alias textWidth: tooltip.width
    id: mouseArea
    acceptedButtons: Qt.NoButton
    anchors.fill: parent
    hoverEnabled: true
    Rectangle {
        id: tooltip
        property int verticalPadding: 1
        property int horizontalPadding: 5
        width: textContainer.width + horizontalPadding * 2
        height: textContainer.height + verticalPadding * 2
        y: -20
        color: "#aa999999"
        Text {
            anchors.centerIn: parent
            id: textContainer
            text: ""
        }
        visible: mouseArea.containsMouse
    }
}
