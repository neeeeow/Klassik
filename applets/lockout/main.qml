/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.private.sessions
import org.kde.kirigami as Kirigami

PlasmoidItem {
    id: root

    preferredRepresentation: fullRepresentation
    fullRepresentation: GridLayout {
        id: grid
        anchors.fill: parent

        readonly property bool horizontal: Plasmoid.formFactor === PlasmaCore.Types.Horizontal
        readonly property int iconSize: Kirigami.Units.iconSizes.small

        rowSpacing: 0
        columnSpacing: 0
        flow: {
            if (horizontal) {
                return ((2 * iconSize) > height) ? GridLayout.LeftToRight : GridLayout.TopToBottom;
            } else {
                return ((2 * iconSize) > width) ? GridLayout.TopToBottom : GridLayout.LeftToRight;
            }
        }

        SessionManagement {
            id: session
        }

        ToolButton {
            id: lock
            flat: false
            Layout.fillWidth: grid.horizontal ? false : true
            Layout.fillHeight: grid.horizontal ? true : false

            onClicked: session["lock"]()
            Kirigami.Icon {
                source: "system-lock-screen"
                width: grid.iconSize
                height: grid.iconSize
                anchors.centerIn: parent
            }
        }

        ToolButton {
            id: logout
            flat: false
            Layout.fillWidth: grid.horizontal ? false : true
            Layout.fillHeight: grid.horizontal ? true : false

            onClicked: session["requestLogoutPrompt"]()
            Kirigami.Icon {
                source: "system-log-out"
                width: grid.iconSize
                height: grid.iconSize
                anchors.centerIn: parent
            }
        }
    }
}
