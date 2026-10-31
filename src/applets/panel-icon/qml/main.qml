/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid

PlasmoidItem {
    id: root

    enabled: Plasmoid.valid
    Plasmoid.onActivated: Plasmoid.run()

    PlasmaCore.Action {
        id: configureAction
        text: i18n("Properties")
        icon.name: "document-properties"
        visible: Plasmoid.valid && Plasmoid.immutability !== PlasmaCore.Types.SystemImmutable
        onTriggered: Plasmoid.configure()
    }
    Component.onCompleted: {
        Plasmoid.setInternalAction("configure", configureAction);
    }
    onExternalData: (mimetype, data) => {
        Plasmoid.url = data;
    }

    PanelButton {
        anchors.fill: parent

        title: Plasmoid.title
        subTitle: Plasmoid.genericName !== Plasmoid.title ? Plasmoid.genericName : ""
        icon.name: Plasmoid.icon

        checkable: Plasmoid.hasMenu
        checked: Plasmoid.menuActive

        onClicked: Plasmoid.run()
    }
}
