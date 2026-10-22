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

    // Copy over the sizing from the gridlayout
    implicitWidth: grid.implicitWidth
    implicitHeight: grid.implicitHeight
    Layout.minimumWidth: grid.Layout.minimumWidth
    Layout.minimumHeight: grid.Layout.minimumHeight
    Layout.preferredWidth: grid.Layout.preferredWidth
    Layout.preferredHeight: grid.Layout.preferredHeight
    Layout.maximumWidth: grid.Layout.maximumWidth
    Layout.maximumHeight: grid.Layout.maximumHeight

    SessionManagement {
        id: session
    }

    GridLayout {
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

        ToolButton {
            id: lock
            flat: false
            Layout.fillWidth: grid.horizontal ? false : true
            Layout.fillHeight: grid.horizontal ? true : false
            implicitWidth: grid.iconSize + 14
            implicitHeight: grid.iconSize + 14

            background: StyledToolButton {
                hoverEnabled: lock.hoverEnabled
                sunken: lock.down || lock.checked
            }

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
            implicitWidth: grid.iconSize + 14
            implicitHeight: grid.iconSize + 14

            background: StyledToolButton {
                hoverEnabled: logout.hoverEnabled
                sunken: logout.down || logout.checked
            }

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
