/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

import QtQuick

import org.kde.plasma.plasmoid

PlasmoidItem{
    id: root

    Plasmoid.icon: Plasmoid.configuration.icon
    Plasmoid.onActivated: Plasmoid.toggleMenu(menuButton)

    PanelMenuButton {
        id: menuButton
        anchors.fill: parent

        title: Plasmoid.title
        icon.name: Plasmoid.icon
        checked: Plasmoid.menuActive

        onClicked: Plasmoid.toggleMenu(menuButton)
    }
}
