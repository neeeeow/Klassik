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
    required property Item applet

    readonly property bool isHorizontal: panel.isHorizontal
    readonly property int handleExtent: grabHandle.handleExtent

    signal dragStarted()
    signal dragMoved(Item handle, real mouseX, real mouseY)
    signal dragFinished()

    Component { id: menuComponent;     PlasmaExtras.Menu {} }
    Component { id: menuItemComponent; PlasmaExtras.MenuItem {} }
    function newMenu(parent)     { return menuComponent.createObject(parent) }
    function newMenuItem(parent) { return menuItemComponent.createObject(parent) }

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
        readonly property bool isMenuOpen: appletMenu.status === PlasmaExtras.Menu.Open

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

            onClicked: appletMenu.show()

            PlasmaExtras.Menu {
                id: appletMenu
                function show(): void {
                    populateMenu();
                    open();
                }
                function populateMenu(): void {
                    clearMenuItems();

                    // Applet actions
                    root.applet.prepareContextualActions();
                    const actions = root.applet.Plasmoid.contextualActions;
                    for (let i = 0; i < actions.length; ++i) {
                        const item = root.newMenuItem(appletMenu);
                        item.action = actions[i];
                    }

                    // Panel menu
                    root.panel.prepareContextualActions();
                    const panelActions = Plasmoid.contextualActions;
                    if (panelActions.length > 0) {
                        const separator = root.newMenuItem(appletMenu);
                        separator.separator = true;

                        const panelMenuItem = root.newMenuItem(appletMenu);
                        panelMenuItem.text = i18n("Panel Menu");

                        const panelMenu = root.newMenu(panelMenuItem);
                        for (let j = 0; j < panelActions.length; ++j) {
                            const item = root.newMenuItem(panelMenu);
                            item.action = panelActions[j];
                        }
                        panelMenu.visualParent = panelMenuItem.action;
                    }
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
