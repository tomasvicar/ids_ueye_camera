/*!
 * \file    main.qml
 * \author  IDS Imaging Development Systems GmbH
 * \date    2019-05-01
 * \since   1.0.0
 *
 * \brief   This application demonstrates how to use the IDS peak API
 *          combined with a QML GUI to display images from Genicam
 *          compatible device.
 *
 * \version 1.0.0
 *
 * Copyright (C) 2019 - 2021, IDS Imaging Development Systems GmbH.
 *
 * The information in this document is subject to change without notice
 * and should not be construed as a commitment by IDS Imaging Development Systems GmbH.
 * IDS Imaging Development Systems GmbH does not assume any responsibility for any errors
 * that may appear in this document.
 *
 * This document, or source code, is provided solely as an example of how to utilize
 * IDS Imaging Development Systems GmbH software libraries in a sample application.
 * IDS Imaging Development Systems GmbH does not assume any responsibility
 * for the use or reliability of any portion of this document.
 *
 * General permission to copy or modify is hereby granted.
 */

import QtQuick 2.2
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import BackEnd 1.0
import ImageItem 1.0

Window {
    id:window
    visible: true
    width: 640
    height: 480
    title: qsTr("simple_live_qml")

    BackEnd {
        id:backend
        onImageChanged: {
            cameraLiveImage.setImage(image)
        }
        onCounterChanged: {
            counterText.text = "Acquired: " + frameCounter + ", errors: " + errorCounter
        }
        Component.onCompleted: {
            versionText.text = "simple_live_qml v" + Version()
            backend.OpenDevice()
        }
        onMessageBoxTrigger: {
            messageBox.title = messageTitle
            messageBox.text = messageText
            messageBox.open()
        }
    }
    
    Dialog {
        id: aboutQtDialog
        title: "About Qt"
        width: 400
        standardButtons: Dialog.Ok
        
        Text {
            id: aboutQtDialogHeader
            text: "About Qt"
            font.weight: Font.Bold
            anchors { top: parent.top; left: parent.left; right: parent.right }
            anchors { topMargin: 10; }
        }
        Text {
            id: aboutQtDialogVersion
            text: "This program uses Qt version " + backend.QtVersion()
            anchors { top: aboutQtDialogHeader.bottom; left: parent.left; right: parent.right }
            anchors { topMargin: 15; }
        }
        Text {
            id: aboutQtDialogLink
            text: "Please see <a href=\"https://qt.io/licensing\">qt.io/licensing</a> for an overview of Qt licensing."
            wrapMode: Text.WordWrap
            anchors { top: aboutQtDialogVersion.bottom; left: parent.left; right: parent.right }
            onLinkActivated: Qt.openUrlExternally(link)
            anchors { topMargin: 10; }
        }
        Text {
            /* empty text element to make sure the Ok-button doesn't overlap the text */
            text: ""
            anchors { top: aboutQtDialogLink.bottom; left: parent.left; right: parent.right }
        }
    }

    Rectangle {
        id: rectangle
        color: "#e4e4e4"
        anchors { top: parent.top; bottom: parent.bottom; left: parent.left; right: parent.right }
        anchors { topMargin: 0; bottomMargin: 24; leftMargin: 0; rightMargin: 0 }

        ImageItem {
            id: cameraLiveImage
            anchors.fill: parent
        }
    }

    Text {
        id: counterText
        text: qsTr("Acquired: -, errors: -")
        anchors.rightMargin: 4
        verticalAlignment: Text.AlignVCenter
        anchors { top: rectangle.bottom; bottom: parent.bottom; left: parent.left; right: versionText.left }
        anchors.margins: 4
    }

    Text {
        id: aboutQtLink
        width: 70
        text: qsTr("<a href=\"#aboutQt\">About Qt</a>")
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        anchors { top: rectangle.bottom; bottom: parent.bottom; right: parent.right }
        anchors.margins: 4
        onLinkActivated: aboutQtDialog.open()
    }

    Text {
        id: versionText
        width: 200
        text: qsTr("Acquired: -, errors: -")
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        anchors { top: rectangle.bottom; bottom: parent.bottom; right: aboutQtLink.left }
        anchors.margins: 4
    }

     MessageDialog {
        id: messageBox
        standardButtons: StandardButton.Ok
    }
}
