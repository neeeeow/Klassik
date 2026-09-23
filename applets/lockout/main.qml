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

    readonly property int minButtonSize: Kirigami.Units.iconSizes.small

    preferredRepresentation: fullRepresentation
    fullRepresentation: GridLayout {
        id: grid

        rowSpacing: 0
        rows: 2
        columns: 1

        SessionManagement {
            id: session
        }

        ToolButton {
            id: lock
            Layout.fillHeight: true
            flat: false

            onClicked: session["lock"]()
            icon.name: "system-lock-screen"
        }

        ToolButton {
            id: logout
            Layout.fillHeight: true
            flat: false

            onClicked: session["requestLogoutPrompt"]()
            icon.name: "system-log-out"
        }
    }
}
