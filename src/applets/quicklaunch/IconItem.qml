/*
 *  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>
 *
 *  SPDX-FileCopyrightText: 2015 David Rosca <nowrep@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick
import org.kde.plasma.plasmoid
import org.kde.draganddrop as DragAndDrop
import org.kde.plasma.extras as PlasmaExtras

Item {
    id: iconItem

    required property string storageId
    required property int index
    required property Logic logic
    required property GridView grid
    required property LauncherModel launcherModel

    readonly property int itemIndex : index
    property bool dragging : false
    readonly property var launcher : logic.launcherData(storageId)
    readonly property string iconName : launcher.iconName || "fork"

    width: grid.cellWidth
    height: grid.cellHeight

    function decreaseIndex() {
        const newIndex = iconItem.itemIndex - 1;
        if (newIndex < 0) {
            return;
        }
        launcherModel.moveId(iconItem.itemIndex, newIndex);
        iconItem.GridView.view.currentIndex = newIndex;
    }

    function increaseIndex() {
        const newIndex = iconItem.itemIndex + 1;
        if (newIndex === (iconItem.GridView.view.count)) {
            return;
        }
        launcherModel.moveId(iconItem.itemIndex, newIndex);
        iconItem.GridView.view.currentIndex = newIndex;
    }

    DragAndDrop.DragArea {
        id: dragArea
        width: Math.min(iconItem.width, iconItem.height)
        height: width
        enabled: !Plasmoid.immutable
        defaultAction: Qt.MoveAction
        supportedActions: Qt.IgnoreAction | Qt.MoveAction
        delegate: panelButton

        mimeData {
            url: iconItem.storageId == "quicklaunch:drop" ? "" : iconItem.logic.storageIdToUrl(iconItem.storageId)
            source: iconItem
        }

        onDragStarted: {
            iconItem.dragging = true;
        }

        onDrop: action => {
            iconItem.dragging = false;
        }

        PanelButton {
            id: panelButton
            anchors.fill: parent
            anchors.margins: 0
            name: iconItem.launcher.applicationName
            icon.name: iconItem.storageId == "quicklaunch:drop" ? "" : iconItem.iconName

            onClicked: iconItem.logic.openLauncher(iconItem.storageId)

            MouseArea {
                id: mouseArea
                anchors.fill: parent
                acceptedButtons: Qt.RightButton
                onPressed: mouse => {
                    contextMenu.refreshActions();
                    contextMenu.open(mouse.x, mouse.y);
                }

                PlasmaExtras.Menu {
                    id: contextMenu

                    property var jumpListItems : []

                    visualParent: mouseArea

                    PlasmaExtras.MenuItem {
                        id: jumpListSeparator
                        separator: true
                    }

                    PlasmaExtras.MenuItem {
                        text: i18nc("@action:inmenu", "Add Launcher…")
                        icon: "list-add"
                        onClicked: iconItem.addLauncher()
                    }

                    PlasmaExtras.MenuItem {
                        text: i18nc("@action:inmenu", "Remove Launcher")
                        icon: "list-remove"
                        onClicked: iconItem.removeLauncher()
                    }

                    PlasmaExtras.MenuItem {
                        separator: true
                    }

                    PlasmaExtras.MenuItem {
                        action: Plasmoid.internalAction("configure")
                    }

                    PlasmaExtras.MenuItem {
                        action: Plasmoid.internalAction("remove")
                    }

                    function refreshActions() {
                        for (var i = 0; i < jumpListItems.length; ++i) {
                            var item = jumpListItems[i];
                            removeMenuItem(item);
                            item.destroy();
                        }
                        jumpListItems = [];

                        for (var i = 0; i < iconItem.launcher.jumpListActions.length; ++i) {
                            var action = iconItem.launcher.jumpListActions[i];
                            var item = menuItemComponent.createObject(iconItem, {
                                "text": action.name,
                                "icon": action.icon
                            });
                            item.clicked.connect((function(actionExec) {
                                return function() {
                                    logic.openExec(actionExec);
                                };
                            })(action.exec));

                            addMenuItem(item, jumpListSeparator);
                            jumpListItems.push(item);
                        }
                    }
                }

                Component {
                    id: menuItemComponent
                    PlasmaExtras.MenuItem { }
                }
            }

        }
    }

    function addLauncher()
    {
        logic.addLauncher();
    }

    function removeLauncher()
    {
        var m = launcherModel;
        m.removeId(itemIndex);
    }
}
