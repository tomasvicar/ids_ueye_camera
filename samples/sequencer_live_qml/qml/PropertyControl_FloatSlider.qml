/*!
 * \file    PropertyControl_FloatSlider.qml
 * \author  IDS Imaging Development Systems GmbH
 * \date    2020-03-01
 * \since   1.1.6
 *
 * \brief   Prototype for a property control slider with spin box,
 *          working with camera property nodes of type 'Float' and 'Integer'
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
    height: contentRow.implicitHeight

    property variant node: dummyNode
    property alias from: slider.minimumValue
    property alias to: slider.maximumValue
    property alias stepSize: slider.stepSize
    property alias value: slider.value
    property alias decimalFactor: slider.decimalFactor
    property alias decimals: slider.decimals
    property alias nameWidth: nameTxt.width
    property bool autoApply: true
    property bool autoUpdate: true
    property bool dirty: false

    property alias displayName: nameTxt.text
    property alias tooltip: nameTxt.tooltip
    property string unit: ""
    property alias dummyNode: dummyNode

    property bool ignoreApply: false

    onNodeChanged: {
        if (!node)
            control.node = Qt.binding(function () {
                return dummyNode
            })
        if (node == dummyNode)
            return

        ignoreApply = true

        if (autoUpdate) {
            standByText.visible = Qt.binding(function () {
                return row.visible ? false : true
            })
            standByText.readable = Qt.binding(function () {
                return node.readable
            })
            slider.enabled = Qt.binding(function () {
                return node.writeable && node.min != node.max
            })
            slider.minimumValue = Qt.binding(function () {
                return node.min
            })
            slider.maximumValue = Qt.binding(function () {
                return node.max
            })
            if (node.inc !== 0)
                slider.stepSize = Qt.binding(function () {
                    return node.inc
                })
            unit = Qt.binding(function () {
                return node.unit ? " " + node.unit : ""
            })
            node.valueChanged.connect(update)
        } else {
            row.visible = true
            standByText.visible = false
            slider.enabled = true
            slider.minimumValue = node.min
            slider.maximumValue = node.max
        }

        if (autoApply) {
            slider.onValueChanged.connect(apply)
        }

        update()

        ignoreApply = false
    }

    function apply() {
        if (ignoreApply) {
            return
        }
        node.value = slider.value
        dirty = true
    }
    function update() {
        ignoreApply = true
        slider.value = node.value
        spinBox.value = node.value
        ignoreApply = false
    }

    Row {
        id: contentRow
        height: implicitHeight
        width: control.width

        Label {
            id: nameTxt
            text: control.node.displayName
                  + (dirty ? "*" : "" /*"\u2713"*/) //+ " " + control.node.accessStatus
            wrapMode: Text.Wrap
            width: 200
            height: implicitHeight
            anchors.verticalCenter: parent.verticalCenter
            property string tooltip: control.node.tooltip + " ("
                                     + control.node.type /*+ " " + control.node.accessStatus*/ + ")"
            ToolTip {
                text: nameTxt.tooltip
            }
        }

        Row {
            id: row
            visible: true
            width: parent.width - nameTxt.width - parent.spacing
            spacing: 5

            Slider {
                id: slider
                width: parent.width - spinBox.width - parent.spacing * 2
                property variant node: control.node
                anchors.verticalCenter: parent.verticalCenter
                minimumValue: 0
                maximumValue: 10
                value: 1000000000000
                property int decimalFactor: 100
                property int decimals: 2
                enabled: node.writeable && minimumValue != maximumValue

                Text {
                    id: minLabel
                    text: slider.minimumValue.toLocaleString(Qt.locale("C"),
                                                             'f',
                                                             slider.decimals)
                    opacity: slider.enabled ? 1 : 0.3
                    anchors.top: parent.bottom
                    anchors.left: parent.left
                    anchors.leftMargin: slider.leftPadding
                    verticalAlignment: Text.AlignBottom
                }

                Text {
                    id: maxLabel
                    text: slider.maximumValue.toLocaleString(Qt.locale("C"),
                                                             'f',
                                                             slider.decimals)
                    opacity: slider.enabled ? 1 : 0.3
                    anchors.top: parent.bottom
                    anchors.right: parent.right
                    anchors.rightMargin: slider.rightPadding
                    verticalAlignment: Text.AlignBottom
                    horizontalAlignment: Text.AlignRight
                }

                ToolTip {
                    enabled: slider.enabled
                    text: slider.value.toLocaleString(Qt.locale("C"), 'f',
                                                      slider.decimals)
                    textOffset: (((slider.value - slider.minimumValue)
                                  / (slider.maximumValue - slider.minimumValue))
                                 * (slider.width - 16)) - ((tip.width - 16) / 2)
                }
            }

            SpinBox {
                id: spinBox
                anchors.verticalCenter: parent.verticalCenter
                minimumValue: slider.minimumValue * decimalFactor
                maximumValue: slider.maximumValue * decimalFactor
                stepSize: Math.max(slider.stepSize, 1) * decimalFactor
                value: 1000000000000 * decimalFactor
                property int decimalFactor: slider.decimalFactor
                decimals: slider.decimals
                enabled: node.writeable && minimumValue != maximumValue
                width: 100
                suffix: unit

                onValueChanged: {
                    slider.value = spinBox.value / decimalFactor
                }
            }
        }

        Label {
            id: standByText
            width: parent.width - nameTxt.width - parent.spacing
            height: implicitHeight
            anchors.verticalCenter: parent.verticalCenter
            property bool readable: true
            text: readable ? (control.value + " " + unit) : "not available"
            opacity: readable ? 1.0 : 0.3
            font.italic: readable ? false : true
            visible: false
        }
    }

    Item {
        id: dummyNode
        property string tooltip: ""
        property string description: ""
        property string name: ""
        property string displayName: ""
        property string unit: ""
        property int min: 0
        property int max: 0
        property int inc: 0
        property int value: 0
        property bool readable: false
        property bool available: false
        property bool writeable: false
    }
}
