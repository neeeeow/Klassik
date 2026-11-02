/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

import QtQuick

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid

PlasmoidItem{
    id: root

    enabled: Plasmoid.hasMenu // if we don't have a menu, something is wrong!'
    Plasmoid.icon: Plasmoid.configuration.icon
    Plasmoid.onActivated: Plasmoid.toggleMenu()

    PlasmaCore.Action {
        id: configureAction
        text: i18n("Edit Menu")
        icon.name: "kmenuedit"
        visible: Plasmoid.hasMenu && Plasmoid.immutability !== PlasmaCore.Types.SystemImmutable
        onTriggered: Plasmoid.menuEditor(Plasmoid.relPath)
    }
    Component.onCompleted: {
        Plasmoid.setInternalAction("configure", configureAction);
    }
    onExternalData: (mimetype, data) => {
        Plasmoid.relPath = data;
    }
    PanelButton {
        anchors.fill: parent

        title: Plasmoid.title
        icon.name: Plasmoid.icon

        checkable: Plasmoid.hasMenu
        checked: Plasmoid.menuActive

        onClicked: Plasmoid.toggleMenu()
    }
}
