/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.extras as PlasmaExtras

import com.github.neeeeow.klassik.painteditems

Item {
    id: root

    required property ContainmentItem panel
    required property PlasmoidItem applet // dndspacer will never have a handle, so use PlasmoidItem here.

    readonly property bool isHorizontal: Plasmoid.formFactor !== PlasmaCore.Types.Vertical
    readonly property int handleExtent: grabHandle.handleExtent

    signal dragStarted()
    signal dragMoved(Item handle, real mouseX, real mouseY)
    signal dragFinished()

    HoverHandler {
        id: hoverHandler
    }

    GridLayout {
        id: appletHandle
        anchors.fill: parent
        rowSpacing: 0
        columnSpacing: 0

        // if the panel is on the bottom or the left, the handle goes first
        readonly property bool reverse: Plasmoid.location === PlasmaCore.Types.BottomEdge || Plasmoid.location === PlasmaCore.Types.RightEdge
        property ContextMenu contextMenu: null
        readonly property bool isMenuOpen: contextMenu?.status === PlasmaExtras.Menu.Open

        visible: Plasmoid.configuration.showAppletHandle || hoverHandler.hovered || grabHandle.pressed || isMenuOpen

        StyledToolButton {
            id: menuButton
            Layout.fillWidth: root.isHorizontal
            Layout.fillHeight: !root.isHorizontal
            Layout.preferredWidth: root.isHorizontal ? -1 : height
            Layout.preferredHeight: root.isHorizontal ? width : -1
            Layout.row: root.isHorizontal ? (appletHandle.reverse ? 0 : 1) : 0
            Layout.column: root.isHorizontal ? 0 : (appletHandle.reverse ? 0 : 1)

            checked: appletHandle.isMenuOpen
            hoverEnabled: true

            contentItem: StyledArrow {
                hovered: menuButton.hovered
                sunken: menuButton.down || menuButton.checked
                direction: {
                    switch (Plasmoid.location) {
                    case PlasmaCore.Types.TopEdge:
                        return StyledArrow.Down;
                    case PlasmaCore.Types.LeftEdge:
                        return StyledArrow.Right;
                    case PlasmaCore.Types.RightEdge:
                        return StyledArrow.Left;
                    default:
                        return StyledArrow.Up;
                    }
                }
            }

            onClicked: {
                if (appletHandle.isMenuOpen)
                    appletHandle.contextMenu.close();
                else {
                    appletHandle.contextMenu?.destroy(); // just in case we have a context menu instance
                    appletHandle.contextMenu = root.panel.openContextMenu(root.applet, menuButton);
                }
            }
        }

        StyledToolBarHandle {
            id: grabHandle
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.row: root.isHorizontal ? (appletHandle.reverse ? 1 : 0) : 0
            Layout.column: root.isHorizontal ? 0 : (appletHandle.reverse ? 1 : 0)
            horizontal: root.isHorizontal
            cursorShape: pressed ? Qt.SizeAllCursor : Qt.ArrowCursor

            onPressed: root.dragStarted()
            onPositionChanged: mouse => {
                if (pressed)
                    root.dragMoved(grabHandle, mouse.x, mouse.y);
            }
            onReleased: root.dragFinished()
            onCanceled: root.dragFinished()
        }
    }
}
