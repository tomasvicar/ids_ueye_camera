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
    title: qsTr("chunks_live_qml")

    BackEnd {
        id: backend
        onImageReceived: {
            cameraLiveImage.setImage(image)

            if (chunkDataExposureTime_ms < 0)
            {
                chunkInfo.text = "Exposure time: not available"
            }
            else
            {
                chunkInfo.text = "Exposure time: " + chunkDataExposureTime_ms + " ms"
            }
        }
        onCountersUpdated: {
            counterText.text = "Acquired: " + frameCounter + ", errors: " + errorCounter
        }
        onMessageBoxTrigger: {
            messageBox.title = messageTitle
            messageBox.text = messageText
            messageBox.open()
        }

        Component.onCompleted: {
            versionText.text = "chunks_live_qml v" + Version()
            if (!backend.start())
            {
                window.close()
            }
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

            Text {
                id: chunkInfo
                color: "red"
                font.pixelSize: Math.sqrt(window.width * window.height) / 30
            }
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
