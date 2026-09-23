/*
 *  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
import org.kde.kirigami as Kirigami

PlasmoidItem{
    id: root

    preferredRepresentation: fullRepresentation

    Plasmoid.icon: Plasmoid.configuration.icon

    fullRepresentation: Button { // We can build the menu button in qml, since Qt Quick hands off buttons to Qt.
        id: menuButton

        flat: true
        checkable: true
        checked: Plasmoid.menuActive

        readonly property bool isHorizontal: Plasmoid.formFactor === PlasmaCore.Types.Horizontal
        readonly property bool isVertical: Plasmoid.formFactor === PlasmaCore.Types.Vertical

        // Fill the height of the container
        Layout.fillWidth: isVertical
        Layout.fillHeight: isHorizontal

        // Make the applet a square
        Layout.preferredWidth: isHorizontal ? height : -1
        Layout.preferredHeight: isVertical ? width : -1

        // Ensure the width/height *never* deviates from our computed values
        Layout.minimumWidth: isHorizontal ? Layout.preferredWidth : -1
        Layout.maximumWidth: isHorizontal ? Layout.preferredWidth : -1
        Layout.minimumHeight: isVertical ? Layout.preferredHeight : -1
        Layout.maximumHeight: isVertical ? Layout.preferredHeight : -1

        background: SunkenAppletFrame {
            id: sunkenFrame
            visible: menuButton.down || menuButton.checked
        }

        contentItem: Kirigami.Icon {
            source: Plasmoid.icon
            active: menuButton.hovered

            scale: (menuButton.down || menuButton.checked) ? (width - sunkenFrame.lineWidth) / width : 1.0
            transformOrigin: Item.Center
        }

        PanelButtonArrow {
            anchors.fill: parent
            active: menuButton.down || menuButton.checked
            location: {
                switch (Plasmoid.location) {
                    case PlasmaCore.Types.TopEdge:
                        return PanelButtonArrow.Below;
                    case PlasmaCore.Types.LeftEdge:
                        return PanelButtonArrow.Right;
                    case PlasmaCore.Types.RightEdge:
                        return PanelButtonArrow.Left;
                    default:
                        return PanelButtonArrow.Above;
                }
            }
        }

        onClicked: Plasmoid.toggleMenu(menuButton, Plasmoid.location)
        Connections {
            target: Plasmoid
            function onActivated() {
                Plasmoid.toggleMenu(menuButton, Plasmoid.location)
            }
        }
    }
}
