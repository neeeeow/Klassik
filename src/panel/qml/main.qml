/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2022 Niccolò Venerandi <niccolo@venerandi.com>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import org.kde.plasma.plasmoid

import org.kde.plasma.core as PlasmaCore
import org.kde.draganddrop as DragDrop

import "LayoutManager.js" as LayoutManager

import com.github.neeeeow.klassik.painteditems

ContainmentItem {
    id: root
    width: 640
    height: 48

//BEGIN properties
    Layout.preferredWidth:  fixedWidth  || currentLayout.implicitWidth  + (isHorizontal ? 2 * panelMargin : 0) // Add width/height for the margin
    Layout.preferredHeight: fixedHeight || currentLayout.implicitHeight + (isHorizontal ? 0 : 2 * panelMargin)
    Layout.fillWidth: {
        return currentLayout.children
            .filter(child => child?.applet?.Plasmoid?.pluginName === "org.kde.plasma.panelspacer")
            .some(child => child.applet.Plasmoid.configuration.expanding)
    }
    Layout.fillHeight: Layout.fillWidth

    readonly property bool isHorizontal: Plasmoid.formFactor !== PlasmaCore.Types.Vertical
    property real fixedWidth: 0
    property real fixedHeight: 0
    property bool hasSpacer: false

    readonly property int panelMargin: 2

    property bool appletRequestsInhibitDnD: false
    property Item dndSpacer: null

    readonly property Component contextMenuComponent: Qt.createComponent("ContextMenu.qml")

    Binding {
        target: root.Plasmoid
        property: "colorize"
        value: Plasmoid.configuration.colorize
    }

//END properties

//BEGIN functions
    function checkLastSpacer(): void {
        for (var i = 0; i < appletsModel.count; ++i) {
            const applet = appletsModel.get(i).applet;
            if (!applet || !applet.visible || !applet.Layout) {
                continue;
            }
            if ((isHorizontal && applet.Layout.fillWidth) ||
                (!isHorizontal && applet.Layout.fillHeight)) {
                    hasSpacer = true;
                return;
            }
        }
        hasSpacer = false;
    }

    function showDndSpacer(x, y): void {
        if (dndSpacer) {
            return; // already showing, so don't show any others
        }
        // Parent to dropArea, since that's what manages the dndSpacer
        dndSpacer = dndSpacerComponent.createObject(dropArea);
        if (!dndSpacer)
            return;
        appletsModel.insert(LayoutManager.indexAtCoordinates(x, y), {applet: dndSpacer});
    }

    function hideDndSpacer(): void {
        if (!dndSpacer) {
            return;
        }
        // Remove it from the appletsModel before we destroy the spacer
        const index = dndSpacer.parent?.index;
        if (typeof index === "number" && index > -1) {
            appletsModel.remove(index);
        }
        dndSpacer.destroy();
        dndSpacer = null;
    }
    function openContextMenu(applet: PlasmoidItem, visualParent: Item, x = undefined, y = undefined): ContextMenu {
        const menu = contextMenuComponent.createObject(visualParent, {
            panel: root,
            applet: applet,
            visualParent: visualParent,
        }) as ContextMenu;
        if (menu)
            menu.show(x, y);
        return menu;
    }
//END functions

//BEGIN connections
    Containment.onAppletAdded: (applet, geometry) => {
        let pos = Qt.point(geometry.x, geometry.y)
        if ("positionBeforeDeletion" in applet) {
            pos = applet.positionBeforeDeletion
            delete applet.positionBeforeDeletion
        }

        LayoutManager.addApplet(applet, pos.x, pos.y);
        root.checkLastSpacer();

        // When a new preset panel is added, avoid calling save() multiple times
        Qt.callLater(LayoutManager.save);
    }

    Containment.onAppletRemoved: (applet) => {
        let plasmoidItem = root.itemFor(applet);

        if (plasmoidItem && plasmoidItem.parent) {
            const pos = Qt.point(plasmoidItem.parent.x, plasmoidItem.parent.y);
            appletsModel.remove(plasmoidItem.parent.index);
            applet["positionBeforeDeletion"] = pos;
        }

        checkLastSpacer();
        LayoutManager.save();
    }

    Plasmoid.onUserConfiguringChanged: {
        if (Plasmoid.userConfiguring) {
            Containment.applets.forEach(applet => {
                const item = root.itemFor(applet);
                if (item)
                    item.expanded = false;
            });
        }
    }

    Plasmoid.onImmutableChanged: {
        // Kill off any active moves if the panel becomes immutable mid drag.
        // Can't imagine how this can happen, but better to be safe.
        if (Plasmoid.immutable) {
            if (cursorOverlay.visible) {
                cursorOverlay.visible = false;
                LayoutManager.save();
            }
            if (globalMove.visible)
                globalMove.finishDrag();
            hideDndSpacer();
            fixedWidth = fixedHeight = 0;
        }
    }

    Connections {
        target: Plasmoid.internalAction("configure klassik")
        function onTriggered(): void {
            configDialog.show()
            configDialog.requestActivate()
        }
    }

//END connections

//BEGIN preferences

    ConfigDialog {
        id: configDialog
    }

//END preferences

//BEGIN background

    Item {
        anchors.fill: parent
        palette: Plasmoid.palette
        Rectangle {
            anchors.fill: parent
            color: palette.window
        }

        Loader {
            anchors.fill: parent
            active: Plasmoid.configuration.useBackground

            sourceComponent: PanelBackgroundImage {
                source: (Plasmoid.configuration.useCustomBackground && Plasmoid.configuration.customBackgroundUrl)
                        ? Plasmoid.configuration.customBackgroundUrl
                        : Qt.resolvedUrl("defaultBackground.png")
                location: Plasmoid.location
                colorize: Plasmoid.configuration.colorize
            }
        }

        Loader {
            anchors.fill: parent
            active: Plasmoid.configuration.drawFrame
            sourceComponent: StyledFrame { lineWidth: root.panelMargin }
        }
    }

//END background

    MouseArea {
        // This area will swallow up contextmenu requests, allowing us to use our custom contextmenu
        id: contextMenuArea
        anchors.fill: parent
        acceptedButtons: Qt.RightButton

        onPressed: mouse => {
            const pos = mapToItem(currentLayout, mouse.x, mouse.y);
            const child = currentLayout.childAt(pos.x, pos.y);
            const clickedApplet = (child && child.applet && child.applet !== root.dndSpacer)
            ? child.applet
            : null;

            root.openContextMenu(clickedApplet, contextMenuArea, mouse.x, mouse.y);
        }

        DragDrop.DropArea {
            id: dropArea

            Component.onCompleted: {
                LayoutManager.root = root;
                LayoutManager.layout = currentLayout;
                LayoutManager.appletsModel = appletsModel;
                LayoutManager.restore();
            }

            onDragEnter: event => {
                if (Plasmoid.immutable || root.appletRequestsInhibitDnD) {
                    event.ignore();
                    return;
                }
                //during drag operations we disable panel auto resize
                root.fixedWidth = root.Layout.preferredWidth;
                root.fixedHeight = root.Layout.preferredHeight;
                root.showDndSpacer(event.x, event.y);
            }

            onDragMove: event => {
                if (Plasmoid.immutable || !root.dndSpacer) return;
                LayoutManager.move(root.dndSpacer.parent, LayoutManager.indexAtCoordinates(event.x, event.y));
            }

            onDragLeave: event => {
                root.hideDndSpacer();
                root.fixedWidth = root.fixedHeight = 0;
            }

            onDrop: event => {
                root.hideDndSpacer();
                if (Plasmoid.immutable) {
                    event.ignore();
                } else {
                    if (!Plasmoid.processMimeData(event.mimeData, event.x, event.y))
                        root.processMimeData(event.mimeData, event.x, event.y);
                    event.accept(event.proposedAction);
                }
                root.fixedWidth = root.fixedHeight = 0;
            }

//BEGIN components
            Component {
                id: appletContainerComponent
                Item {
                    id: container

                    palette: Plasmoid.palette

                    required property Item applet
                    required property int index
                    visible: applet?.Plasmoid?.status !== PlasmaCore.Types.HiddenStatus || (!Plasmoid.immutable && Plasmoid.userConfiguring) || (Containment.corona?.editMode ?? false);

                    readonly property bool wantsToFillWidth: applet?.Layout.fillWidth ?? false
                    readonly property bool wantsToFillHeight: applet?.Layout.fillHeight ?? false

                    readonly property real availWidth: root.width - Layout.leftMargin - Layout.rightMargin
                    readonly property real availHeight: root.height - Layout.topMargin - Layout.bottomMargin

                    readonly property real margin:
                        applet?.Plasmoid && !(applet.Plasmoid.constraintHints & Plasmoid.CanFillArea) ? root.panelMargin : 0

                    Layout.topMargin:    root.isHorizontal ? margin : 0
                    Layout.bottomMargin: root.isHorizontal ? margin : 0
                    Layout.leftMargin:   root.isHorizontal ? 0 : margin
                    Layout.rightMargin:  root.isHorizontal ? 0 : margin

                    // Always fill width/height, in order to still shrink the applet when there is not enough space.
                    // When the applet doesn't want to expand set a Layout.maximumWidth accordingly
                    // https://bugs.kde.org/show_bug.cgi?id=473420
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    onWantsToFillWidthChanged: root.checkLastSpacer()
                    onWantsToFillHeightChanged: root.checkLastSpacer()

                    function findPositive(first, second) {return first > 0 ? first : second}

                    // Handle geometry
                    readonly property bool showHandle: !Plasmoid.immutable
                                                    && applet !== null
                                                    && applet !== root.dndSpacer
                                                    && applet?.Plasmoid?.metaData?.category !== "Application Launchers"
                                                    && applet?.Plasmoid?.pluginName !== "org.kde.plasma.icon"
                                                    && applet?.Plasmoid?.pluginName !== "com.github.neeeeow.klassik.panel.icon"
                    readonly property int handleExtent: (handleLoader.item as AppletHandle)?.handleExtent ?? 0

                    // Compute applet geometry
                    readonly property real appletMinW: findPositive(applet?.Layout.minimumWidth, availHeight)
                    readonly property real appletMinH: findPositive(applet?.Layout.minimumHeight, availWidth)
                    readonly property real appletPrefW: findPositive(applet?.Layout.preferredWidth, appletMinW)
                    readonly property real appletPrefH: findPositive(applet?.Layout.preferredHeight, appletMinH)
                    Layout.minimumWidth: root.isHorizontal ? appletMinW + handleExtent : availWidth
                    Layout.minimumHeight: !root.isHorizontal ? appletMinH + handleExtent : availHeight
                    Layout.preferredWidth: root.isHorizontal ? appletPrefW + handleExtent : availWidth
                    Layout.preferredHeight: !root.isHorizontal ? appletPrefH + handleExtent : availHeight
                    Layout.maximumWidth: root.isHorizontal ? (wantsToFillWidth ? findPositive(applet?.Layout.maximumWidth, root.width) : Math.min(applet?.Layout.maximumWidth, appletPrefW)) + handleExtent : availWidth
                    Layout.maximumHeight: !root.isHorizontal ? (wantsToFillHeight ? findPositive(applet?.Layout.maximumHeight, root.height) : Math.min(applet?.Layout.maximumHeight, appletPrefH)) + handleExtent : availHeight

                    Loader {
                        id: handleLoader
                        active: container.showHandle
                        width: root.isHorizontal ? container.handleExtent : container.width
                        height: root.isHorizontal ? container.height : container.handleExtent
                        anchors.top: parent.top
                        anchors.left: parent.left

                        sourceComponent: AppletHandle {
                            anchors.fill: parent
                            panel: root
                            applet: container.applet as PlasmoidItem

                            onDragStarted: cursorOverlay.visible = true
                            onDragMoved: (handle, mouseX, mouseY) => {
                                const globalPos = handle.mapToItem(currentLayout, mouseX, mouseY);
                                const targetIndex = LayoutManager.indexAtCoordinates(globalPos.x, globalPos.y);
                                LayoutManager.move(container, targetIndex);
                            }
                            onDragFinished: {
                                cursorOverlay.visible = false;
                                LayoutManager.save();
                            }
                        }
                    }

                    // Custom move action
                    PlasmaCore.Action {
                        id: contextMenuMoveAction
                        text: i18n("Move %1", container.applet?.Plasmoid?.title ?? "")
                        icon.name: "transform-move"
                        enabled: !Plasmoid.immutable // only allow the applet to be moved if the panel isn't immutable
                        visible: enabled
                        onTriggered: globalMove.startDrag(container)
                    }

                    onAppletChanged: {
                        if (applet) {
                            applet.parent = container;
                            applet.anchors.fill = container;
                            applet.anchors.leftMargin = Qt.binding(() => root.isHorizontal ? container.handleExtent : 0);
                            applet.anchors.topMargin  = Qt.binding(() => root.isHorizontal ? 0 : container.handleExtent);

                            // Add our custom move action to the applet's internal actions
                            if (applet !== root.dndSpacer) {
                                applet.Plasmoid.setInternalAction("move", contextMenuMoveAction);
                            }
                        } else {
                            appletsModel.remove(index);
                        }
                    }
                }
            }

            Component {
                id: dndSpacerComponent
                StyledFocusRect {}
            }
//END components

//BEGIN UI elements

            anchors {
                fill: parent
                leftMargin: root.isHorizontal ? root.panelMargin : 0
                rightMargin: root.isHorizontal ? root.panelMargin : 0
                topMargin: root.isHorizontal ? 0 : root.panelMargin
                bottomMargin: root.isHorizontal ? 0 : root.panelMargin
            }

            ListModel {
                id: appletsModel
            }

            GridLayout {
                id: currentLayout

                Repeater {
                    model: appletsModel
                    delegate: appletContainerComponent
                }

                anchors.fill: parent
                rowSpacing: 0
                columnSpacing: 0

        // BEGIN BUG 454095: use lastSpacer to left align applets, as implicitWidth is updated too late
                Item {
                    id: lastSpacer
                    visible: !root.hasSpacer
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    /**
                    * This index will be used when adding a new panel.
                    *
                    * @see LayoutManager.indexAtCoordinates
                    */
                    readonly property alias index: appletsModel.count
                }
        // END BUG 454095

                rows: root.isHorizontal ? 1 : currentLayout.children.length
                columns: root.isHorizontal ? currentLayout.children.length : 1
                flow: root.isHorizontal ? GridLayout.LeftToRight : GridLayout.TopToBottom
                layoutDirection: Application.layoutDirection
            }
        }
    }
//END UI elements

    // Simple overlay which allows Qt.SizeAllCursor to displayed all across the panel whenever an applet is being dragged by the grabber
    MouseArea {
        id: cursorOverlay
        anchors.fill: root
        enabled: false
        visible: false
        z: 9999
        cursorShape: Qt.SizeAllCursor
        acceptedButtons: Qt.NoButton
    }

    // Mouse area which moves applets whenever a Move action is selected
    MouseArea {
        id: globalMove
        anchors.fill: root
        enabled: false
        visible: false
        z: 9998
        hoverEnabled: true
        cursorShape: Qt.SizeAllCursor
        acceptedButtons: Qt.AllButtons

        property Item targetContainer: null // the container which we're dragging
        onTargetContainerChanged: {
            if (!targetContainer && visible)
                finishDrag();
        }

        function startDrag(containerItem: Item): void {
            if (Plasmoid.immutable) return; // we never want to move things on an immutable panel
            targetContainer = containerItem;
            enabled = true;
            visible = true;
        }
        function finishDrag(): void {
            enabled = false;
            visible = false;
            targetContainer = null;
            LayoutManager.save();
        }

        onPositionChanged: mouse => {
            if (!targetContainer)
                return;
            const pos = mapToItem(currentLayout, mouse.x, mouse.y);
            const targetIndex = LayoutManager.indexAtCoordinates(pos.x, pos.y);
            LayoutManager.move(targetContainer, targetIndex);
        }
        onClicked: {
            finishDrag();
        }
        onCanceled: {
            finishDrag();
        }
    }
}
