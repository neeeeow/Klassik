/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore

PlasmoidItem{
    id: root

    Plasmoid.icon: Plasmoid.configuration.icon
    Plasmoid.onActivated: Plasmoid.toggleMenu(root.fullRepresentationItem)

    PanelMenuButton {
        id: menuButton
        anchors.fill: parent

        name: Plasmoid.title
        icon.name: Plasmoid.icon
        checked: Plasmoid.menuActive
        onClicked: Plasmoid.toggleMenu(menuButton)

        readonly property bool horizontal: Plasmoid.formFactor === PlasmaCore.Types.Horizontal
        readonly property bool vertical: Plasmoid.formFactor === PlasmaCore.Types.Vertical
        readonly property bool floating: (!horizontal) && (!vertical)

        // Fill the height of the container
        Layout.fillWidth: vertical || floating
        Layout.fillHeight: horizontal || floating

        // Make the applet a square
        Layout.preferredWidth: horizontal ? height : -1
        Layout.preferredHeight: vertical ? width : -1

        // Ensure the width/height *never* deviates from our computed values
        Layout.minimumWidth: horizontal ? Layout.preferredWidth : -1
        Layout.maximumWidth: horizontal ? Layout.preferredWidth : -1
        Layout.minimumHeight: vertical ? Layout.preferredHeight : -1
        Layout.maximumHeight: vertical ? Layout.preferredHeight : -1
    }
}
