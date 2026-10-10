/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
import org.kde.kirigami as Kirigami

PlasmoidItem{
    id: root

    Plasmoid.icon: Plasmoid.configuration.icon

    Connections {
        target: Plasmoid
        function onActivated() {
            Plasmoid.toggleMenu(menuButton)
        }
    }

    preferredRepresentation: fullRepresentation
    fullRepresentation: Button { // We can build the menu button in qml, since Qt Quick hands off buttons to Qt.
        id: menuButton

        flat: true
        checkable: true
        checked: Plasmoid.menuActive

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

        background: StyledFrame {
            id: sunkenFrame
            visible: menuButton.down || menuButton.checked
            sunken: true
        }

        PlasmaCore.ToolTipArea {
            anchors.fill: parent
            mainText: Plasmoid.title
            icon: Plasmoid.icon
        }

        contentItem: Item {
            Kirigami.Icon {
                anchors.fill: parent
                source: Plasmoid.icon
                active: menuButton.hovered

                scale: (menuButton.down || menuButton.checked) ? (width - 2*sunkenFrame.lineWidth) / width : 1.0
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
        }

        onClicked: Plasmoid.toggleMenu(menuButton)
    }
}
