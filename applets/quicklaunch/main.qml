/*
 *  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>
 *
 *  SPDX-FileCopyrightText: 2015 David Rosca <nowrep@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
import org.kde.kirigami as Kirigami
import org.kde.plasma.components as PlasmaComponents3
import org.kde.draganddrop as DragAndDrop
import plasma.applet.com.github.neeeeow.klassik.quicklaunch

import "layout.js" as LayoutManager

PlasmoidItem {
    id: root

    readonly property int sectionCount: Plasmoid.configuration.sectionCount
    readonly property bool vertical : Plasmoid.formFactor == PlasmaCore.Types.Vertical || (Plasmoid.formFactor == PlasmaCore.Types.Planar && height > width)
    readonly property bool horizontal : Plasmoid.formFactor == PlasmaCore.Types.Horizontal
    property bool dragging : false
    property int internalDragIndex: -1
    property int internalDragOriginalIndex: -1

    // Set up the layout
    Layout.fillWidth: vertical
    Layout.fillHeight: horizontal

    // If the grid is empty, reserve space for the add launchers icon
    Layout.preferredWidth: horizontal ? ((grid.count > 0) ? LayoutManager.preferredExtent() : parent.height) : -1
    Layout.preferredHeight: vertical ? ((grid.count > 0) ? LayoutManager.preferredExtent() : parent.width) : -1

    Layout.minimumWidth: horizontal ? Layout.preferredWidth : -1
    Layout.maximumWidth: horizontal ? Layout.preferredWidth : -1
    Layout.minimumHeight: vertical ? Layout.preferredHeight : -1
    Layout.maximumHeight: vertical ? Layout.preferredHeight : -1

    preferredRepresentation: fullRepresentation
    Plasmoid.backgroundHints: PlasmaCore.Types.DefaultBackground | PlasmaCore.Types.ConfigurableBackground

    Item {
        id: launcher
        anchors.fill: parent

        // Quicklauncher grid
        GridView {
            id: grid
            anchors.fill: parent
            interactive: false
            flow: root.horizontal ? GridView.FlowTopToBottom : GridView.FlowLeftToRight
            cellWidth: LayoutManager.preferredCellExtent()
            cellHeight: LayoutManager.preferredCellExtent()
            visible: count

            model: LauncherModel {
                id: launcherModel
            }

            delegate: IconItem {
                logic: logic
                grid: grid
                launcherModel: launcherModel
            }
        }

        Kirigami.Icon {
            id: defaultIcon
            anchors.fill: parent
            source: "fork"
            visible: !grid.visible

            PlasmaCore.ToolTipArea {
                anchors.fill: parent
                mainText: i18n("Quicklaunch")
                subText: i18nc("@info", "Add launchers using the context menu.")
                location: Plasmoid.location
            }
        }

        DragAndDrop.DropArea {
            id: dropArea
            anchors.fill: parent
            preventStealing: true
            enabled: !Plasmoid.immutable

            function gridIndexAt(eventX, eventY) {
                var pos = grid.mapFromItem(dropArea, eventX, eventY);
                return grid.indexAt(pos.x, pos.y);
            }

            onDragEnter: event => {
                if (event.mimeData.hasUrls) {
                    root.dragging = true;
                    root.internalDragIndex = -1;
                    root.internalDragOriginalIndex = -1;

                    var urls = event.mimeData.urls;
                    if (urls.length === 1) {
                        var dragId = logic.urlToStorageId(urls[0]);
                        var modelIds = launcherModel.ids();
                        for (var i = 0; i < modelIds.length; ++i) {
                            if (modelIds[i] === dragId) {
                                root.internalDragIndex = i;
                                root.internalDragOriginalIndex = i;
                                break;
                            }
                        }
                    }
                } else {
                    event.ignore();
                }
            }

            onDragMove: event => {
                var index = dropArea.gridIndexAt(event.x, event.y);

                if (root.internalDragIndex >= 0) {
                    if (index >= 0 && index !== root.internalDragIndex) {
                        launcherModel.move(root.internalDragIndex, index, 1);
                        root.internalDragIndex = index;
                    }
                } else {
                    launcherModel.showDropMarker(index);
                }
            }

            onDragLeave: {
                root.dragging = false;
                if (root.internalDragIndex >= 0) {
                    if (root.internalDragIndex !== root.internalDragOriginalIndex) {
                        launcherModel.move(root.internalDragIndex, root.internalDragOriginalIndex, 1);
                    }
                    root.internalDragIndex = -1;
                    root.internalDragOriginalIndex = -1;
                } else {
                    launcherModel.clearDropMarker();
                }
            }

            onDrop: event => {
                root.dragging = false;

                if (root.internalDragIndex >= 0) {
                    root.internalDragIndex = -1;
                    root.internalDragOriginalIndex = -1;
                    event.accept(Qt.IgnoreAction);
                    root.saveConfiguration();
                } else {
                    var index = dropArea.gridIndexAt(event.x, event.y);
                    launcherModel.clearDropMarker();
                    var urls = event.mimeData.urls;
                    event.accept(event.proposedAction);
                    Qt.callLater(function() {
                        var ids = [];
                        for (var i = 0; i < urls.length; ++i) {
                            var id = logic.urlToStorageId(urls[i]);
                            if (id.length) {
                                ids.push(id);
                            }
                        }
                        launcherModel.insertIds(index == -1 ? launcherModel.count : index, ids);
                    });
                }
            }
        }
    }

    Logic {
        id: logic

        onLauncherAdded: (storageId) => {
            launcherModel.appendId(storageId);
        }
    }

    Connections {
        target: Plasmoid.configuration
        function onLauncherIdsChanged() {
            if (root.dragging) return;
            var configIds = Plasmoid.configuration.launcherIds;
            var modelIds = launcherModel.ids();
            if (configIds.length === modelIds.length) {
                var same = true;
                for (var i = 0; i < configIds.length; ++i) {
                    if (configIds[i] !== modelIds[i]) {
                        same = false;
                        break;
                    }
                }
                if (same) return;
            }
            launcherModel.idsChanged.disconnect(root.saveConfiguration);
            launcherModel.setIds(Plasmoid.configuration.launcherIds);
            launcherModel.idsChanged.connect(root.saveConfiguration);
        }
    }

    Plasmoid.contextualActions: [
        PlasmaCore.Action {
            text: i18nc("@action", "Add Launcher…")
            icon.name: "list-add"
            onTriggered: logic.addLauncher()
        }
    ]

    Component.onCompleted: {
        launcherModel.setIds(Plasmoid.configuration.launcherIds);
        launcherModel.idsChanged.connect(saveConfiguration);
    }

    function saveConfiguration()
    {
        if (!dragging) {
            Plasmoid.configuration.launcherIds = launcherModel.ids();
        }
    }

    function addLauncherUrl(url)
    {
        // This function is neeeded to allow us to add launchers externally
        var storageId = logic.urlToStorageId(url);
        launcherModel.appendId(storageId);
    }
}
