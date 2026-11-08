/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

import QtQuick

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid

PlasmoidItem {
    id: root

    enabled: Plasmoid.hasMenu
    Plasmoid.icon: Plasmoid.configuration.icon
    Plasmoid.onActivated: Plasmoid.toggleMenu()

    ServiceMenuButton {
        anchors.fill: parent

        title: Plasmoid.title
        icon.name: Plasmoid.icon

        onClicked: Plasmoid.toggleMenu()
    }
}
