/*!
 * \file    Dialog_RoiSize.qml
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-03-01
 * \since   1.1.6
 *
 * \brief   Window to configure ROI size (width and height)
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
    id: dialogRoiSize
    title: "ROI Size"
    visible: true
    width: 400
    height: 480
    modality: Qt.ApplicationModal
    property int imageWidthMax: 1
    property int imageHeightMax: 1
    property alias imageWidth: widthSlider.value
    property alias imageHeight: heightSlider.value
    standardButtons: Dialog.Cancel | Dialog.Ok

    function update() {
        widthSlider.value = widthSlider.node.value
        heightSlider.value = heightSlider.node.value
    }

    PropertyContainer {
        id: roiControlContainer
        anchors.fill: parent
        title: "Region of Interest"
        content: [

            Row {
                id: contentRowWidth
                height: implicitHeight
                width: parent.width - 2 * parent.padding
                spacing: 8

                Label {
                    id: nameLabelWidth
                    text: "Width"
                    anchors.verticalCenter: parent.verticalCenter
                }

                Slider {
                    id: widthSlider
                    width: parent.width - parent.spacing * 2
                           - nameLabelWidth.width - valueLabel.width
                    property var node: backEnd.nodeList.get("Width")
                    minimumValue: node ? node.min : 0
                    maximumValue: node ? node.max : 0
                    stepSize: node ? node.inc : 0
                    value: node.value
                    property int decimals: 0
                    property int decimalFactor: 1
                }
                Label {
                    id: valueLabel
                    text: widthSlider.value
                    anchors.verticalCenter: parent.verticalCenter
                }
            },

            Row {
                id: contentRowHeight
                height: implicitHeight
                width: parent.width - 2 * parent.padding
                spacing: 8

                Label {
                    id: nameLabelHeight
                    text: "Height"
                    anchors.verticalCenter: parent.verticalCenter
                }

                Slider {
                    id: heightSlider
                    width: parent.width - parent.spacing * 2
                           - nameLabelHeight.width - valueLabelHeight.width
                    property var node: backEnd.nodeList.get("Height")
                    minimumValue: node ? node.min : 0
                    maximumValue: node ? node.max : 0
                    stepSize: node ? node.inc : 0
                    value: node.value
                    property int decimals: 0
                    property int decimalFactor: 1
                }
                Label {
                    id: valueLabelHeight
                    text: heightSlider.value
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        ]
    }
}
