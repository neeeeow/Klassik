/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
pragma ComponentBehavior: Bound

import QtQuick

MouseArea {
    id: area

    property alias horizontal: handle.horizontal
    readonly property alias handleExtent: handle.handleExtent

    acceptedButtons: Qt.LeftButton
    hoverEnabled: true

    implicitWidth: horizontal ? handle.handleExtent : 0
    implicitHeight: horizontal ? 0 : handle.handleExtent

    ToolBarHandle {
        id: handle
        anchors.fill: parent
        hovered: area.containsMouse
        sunken: area.pressed
    }
}
