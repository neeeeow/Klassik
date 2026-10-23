/*
   SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

   SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls

import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
import org.kde.kirigami as Kirigami

Button {
    id: root

    required property string name

    flat: true
    hoverEnabled: true

    background: StyledFrame {
        id: frame
        visible: root.down || root.checked
        sunken: true
    }

    contentItem: Kirigami.Icon {
        anchors.fill: parent
        source: root.icon.name
        active: root.hovered

        scale: (root.down || root.checked) ? (width - 2*frame.lineWidth) / width : 1.0
        transformOrigin: Item.Center
    }

    PlasmaCore.ToolTipArea {
        anchors.fill: parent
        mainText: root.name
        icon: root.icon.name
    }
}
