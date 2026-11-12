/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick

import org.kde.plasma.plasmoid

PlasmoidItem {
    id: root

    enabled: Plasmoid.valid
    Plasmoid.onActivated: Plasmoid.run()

    onExternalData: (mimetype, data) => {
        Plasmoid.url = data;
    }

    ServiceMenuButton {
        anchors.fill: parent

        title: Plasmoid.title
        subTitle: Plasmoid.genericName !== Plasmoid.title ? Plasmoid.genericName : ""
        icon.name: Plasmoid.icon

        onClicked: Plasmoid.run()
    }
}
