/*!
 * \file    Window_Image.qml
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-03-01
 * \since   1.1.6
 *
 * \brief   Prototype for a window showing the camera image
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

import Sequencer.ImageItem 1.0

Window {
    id: windowImage
    title: "Camera Image"
    visible: true
    width: 640
    height: 480
    modality: Qt.NonModal
    property bool enabled: true
    property alias liveImage: liveImage
    property int imageId: -1
    property bool visibleOnNewImage: false

    function sourceImage() {
        return liveImage.sourceImage
    }

    ImageItem {
        id: liveImage
        width: sourceSize.width
        height: sourceSize.height
        transformOrigin: Item.Center
        anchors.centerIn: parent
        boundsSize: Qt.size(windowImage.width, windowImage.height)
    }

    Label {
        id: timestampLabel
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: 20
    }

    Connections {
        target: backEnd
        onImageReceived: {
            if (windowImage.enabled) {
                if ((imageId === -1) || (iterator === imageId)) {
                    liveImage.setImage(image)
                    timestampLabel.text = "TS: " + timestamp + " us (Δ: " + timestampDelta + " us)"
                    if (visibleOnNewImage && (windowImage.visible === false))
                        windowImage.visible = true
                }
            }
        }

        onClosing: {

        }
    }
}
