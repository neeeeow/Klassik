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

    readonly property int maxSectionCount: Plasmoid.configuration.maxSectionCount
    readonly property bool showLauncherNames : Plasmoid.configuration.showLauncherNames
    readonly property string title : Plasmoid.formFactor == PlasmaCore.Types.Planar ? Plasmoid.configuration.title : ""
    readonly property bool vertical : Plasmoid.formFactor == PlasmaCore.Types.Vertical || (Plasmoid.formFactor == PlasmaCore.Types.Planar && height > width)
    readonly property bool horizontal : Plasmoid.formFactor == PlasmaCore.Types.Horizontal
    property bool dragging : false
    property int internalDragIndex: -1
    property int internalDragOriginalIndex: -1

    Layout.minimumWidth: LayoutManager.minimumWidth()
    Layout.minimumHeight: LayoutManager.minimumHeight()
    Layout.preferredWidth: LayoutManager.preferredWidth()
    Layout.preferredHeight: LayoutManager.preferredHeight()

    preferredRepresentation: fullRepresentation
    Plasmoid.backgroundHints: PlasmaCore.Types.DefaultBackground | PlasmaCore.Types.ConfigurableBackground

    Item {
        anchors.fill: parent

        PlasmaComponents3.Label {
            id: titleLabel

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }

            height: Kirigami.Units.iconSizes.sizeForLabels
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignTop
            elide: Text.ElideMiddle
            text: root.title
            textFormat: Text.PlainText
        }

        Item {
            id: launcher

            anchors {
                top: root.title.length ? titleLabel.bottom : parent.top
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            GridView {
                id: grid
                anchors.fill: parent
                interactive: false
                flow: root.horizontal ? GridView.FlowTopToBottom : GridView.FlowLeftToRight
                cellWidth: LayoutManager.preferredCellWidth()
                cellHeight: LayoutManager.preferredCellHeight()
                visible: count

                model: UrlModel {
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
                    subText: i18nc("@info", "Add launchers by Drag and Drop or by using the context menu.")
                    location: Plasmoid.location
                }
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
                        var dragUrl = urls[0].toString();
                        var modelUrls = launcherModel.urls();
                        for (var i = 0; i < modelUrls.length; ++i) {
                            if (modelUrls[i].toString() === dragUrl) {
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
                        launcherModel.insertUrls(index == -1 ? launcherModel.count : index, urls);
                    });
                }
            }
        }
    }

    Logic {
        id: logic

        onLauncherAdded: (url) => {
            launcherModel.appendUrl(url);
        }

        onLauncherEdited: (url, index) => {
            launcherModel.changeUrl(index, url);
        }
    }

    Connections {
        target: Plasmoid.configuration
       function onLauncherUrlsChanged() {
            if (root.dragging) return;
            var configUrls = Plasmoid.configuration.launcherUrls;
            var modelUrls = launcherModel.urls();
            if (configUrls.length === modelUrls.length) {
                var same = true;
                for (var i = 0; i < configUrls.length; ++i) {
                    if (configUrls[i].toString() !== modelUrls[i].toString()) {
                        same = false;
                        break;
                    }
                }
                if (same) return;
            }
            launcherModel.urlsChanged.disconnect(root.saveConfiguration);
            launcherModel.setUrls(Plasmoid.configuration.launcherUrls);
            launcherModel.urlsChanged.connect(root.saveConfiguration);
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
        launcherModel.setUrls(Plasmoid.configuration.launcherUrls);
        launcherModel.urlsChanged.connect(saveConfiguration);
    }

    function saveConfiguration()
    {
        if (!dragging) {
            Plasmoid.configuration.launcherUrls = launcherModel.urls();
        }
    }

    function addLauncherUrl(url)
    {
        // This function is neeeded to allow us to add launchers externally
        launcherModel.appendUrl(url);
    }
}
