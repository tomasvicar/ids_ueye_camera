/*!
 * \file    Dialog_SequencerTrigger.qml
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-03-01
 * \since   1.1.6
 *
 * \brief   Prototype for sequencer trigger dialog to (software) trigger
 *          an image sequence and save the sequence to a specified folder
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
import QtQuick.Dialogs 1.2

Dialog {
    id: dialogSequencerTrigger
    title: "Trigger Sequencer"
    width: 300
    height: 300
    modality: Qt.NonModal
    standardButtons: Dialog.Close
    property var sequencerDuration

    signal triggered
    signal saveClicked

    Component.onCompleted: {
        sequencerDuration = backEnd.sequencerDuration
    }

    Column {
        anchors.centerIn: parent
        spacing: 20
        Button {
            text: "Trigger Sequence again"
            onClicked: {
                triggered()
            }
        }
        Button {
            id: saveImageButton
            text: "Save images"
            onClicked: {
                saveClicked()
            }
        }
    }
}
