/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
import org.kde.kirigami as Kirigami

import com.github.neeeeow.klassik.painteditems

Button {
    id: button

    property alias title: tooltip.mainText
    property alias subTitle: tooltip.subText

    flat: true
    hoverEnabled: true
    checked: Plasmoid.active

    background: StyledFrame {
        id: frame
        visible: button.down || button.checked
        sunken: true
        hovered: button.hovered
    }

    contentItem: Item {
        Kirigami.Icon {
            anchors.fill: parent
            source: button.icon.name
            active: button.hovered

            scale: ((button.down || button.checked) && (width > 0)) ? Math.max((width - 2 * frame.lineWidth), 0) / width : 1.0
            transformOrigin: Item.Center
        }
        Loader {
            anchors.fill: parent
            active: Plasmoid.hasMenu

            sourceComponent: PanelButtonArrow {
                location: Plasmoid.location
                sunken: button.down || button.checked
                hovered: button.hovered
            }
        }
    }

    PlasmaCore.ToolTipArea {
        id: tooltip
        anchors.fill: parent
        active: !button.checked
        icon: button.icon.name
    }
}
