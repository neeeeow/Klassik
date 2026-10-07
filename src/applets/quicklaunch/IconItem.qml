/*
 *  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>
 *
 *  SPDX-FileCopyrightText: 2015 David Rosca <nowrep@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick
import org.kde.plasma.core as PlasmaCore
import org.kde.kirigami as Kirigami
import org.kde.plasma.plasmoid
import org.kde.draganddrop as DragAndDrop
import org.kde.plasma.extras as PlasmaExtras
import plasma.applet.com.github.neeeeow.klassik.quicklaunch

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
        delegate: icon

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

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            anchors.margins: 0
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            readonly property bool isDown: pressed && containsMouse

            activeFocusOnTab: true
            Accessible.name: iconItem.launcher.applicationName
            Accessible.description: i18n("Launch %1", iconItem.launcher.genericName || iconItem.launcher.applicationName)
            Accessible.role: Accessible.Button

            onPressed: mouse => {
                if (mouse.button == Qt.RightButton) {
                    contextMenu.refreshActions();
                    contextMenu.open(mouse.x, mouse.y);
                }
            }

            onClicked: mouse => {
                if (mouse.button == Qt.LeftButton) {
                    iconItem.logic.openLauncher(iconItem.storageId)
                }
            }

            StyledFrame {
                id: sunkenFrame
                anchors.fill: parent
                visible: mouseArea.isDown
                sunken: true
            }

            Kirigami.Icon {
                id: icon
                anchors.fill: parent

                source: iconItem.storageId == "quicklaunch:drop" ? "" : iconItem.iconName
                active: mouseArea.containsMouse

                scale: mouseArea.isDown ? (width - 2*sunkenFrame.lineWidth) / width : 1.0
                transformOrigin: Item.Center
            }

            PlasmaCore.ToolTipArea {
                anchors.fill: parent
                active: !iconItem.dragging
                mainText: iconItem.launcher.applicationName
                subText: iconItem.launcher.genericName
                icon: iconItem.iconName
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
