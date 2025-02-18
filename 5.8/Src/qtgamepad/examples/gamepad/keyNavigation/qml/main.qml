/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Gamepad module
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/


import QtQuick 2.2
import QtQuick.Controls 1.1
import QtGamepad 1.0

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Gamepad Key Navigation")

    Gamepad {
        id: gamepad1
        deviceId: GamepadManager.connectedGamepads.length > 0 ? GamepadManager.connectedGamepads[0] : -1
    }

    Connections {
        target: GamepadManager
        onGamepadConnected: gamepad1.deviceId = deviceId
    }

    GamepadKeyNavigation {
        id: gamepadKeyNavigation
        gamepad: gamepad1
        active: true
        buttonYKey: Qt.Key_Y
    }

    Text {
        text: qsTr("Test key events from Gamepad")
        anchors.centerIn: parent
        focus: true
        Keys.onPressed: {
            if (event.key === Qt.Key_Up) {
                text = "Key Up Pressed";
            } else if (event.key === Qt.Key_Down) {
                text = "Key Down Pressed";
            } else if (event.key === Qt.Key_Right) {
                text = "Key Right Pressed";
            } else if (event.key === Qt.Key_Left) {
                text = "Key Left Pressed";
            } else if (event.key === Qt.Key_Return) {
                text = "Key Return Pressed";
            } else if (event.key === Qt.Key_Back) {
                text = "Key Back Pressed";
            } else if (event.key === Qt.Key_Forward) {
                text = "Key Forward Pressed";
            } else if (event.key === Qt.Key_Y) {
                text = "Key Y Pressed";
            }
        }
        Keys.onReleased: {
            if (event.key === Qt.Key_Up) {
                text = "Key Up Released";
            } else if (event.key === Qt.Key_Down) {
                text = "Key Down Released";
            } else if (event.key === Qt.Key_Right) {
                text = "Key Right Released";
            } else if (event.key === Qt.Key_Left) {
                text = "Key Left Released";
            } else if (event.key === Qt.Key_Return) {
                text = "Key Return Released";
            } else if (event.key === Qt.Key_Back) {
                text = "Key Back Released";
            } else if (event.key === Qt.Key_Forward) {
                text = "Key Forward Released";
            } else if (event.key === Qt.Key_Y) {
                text = "Key Y Released";
            }
        }
    }
}
