/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2022 Niccolò Venerandi <niccolo@venerandi.com>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import org.kde.plasma.plasmoid

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.extras as PlasmaExtras
import org.kde.draganddrop as DragDrop
import org.kde.kirigami as Kirigami

import "LayoutManager.js" as LayoutManager

ContainmentItem {
    id: root
    width: 640
    height: 48

//BEGIN properties
    Layout.preferredWidth:  fixedWidth  || currentLayout.implicitWidth  + (isHorizontal ? 2 * panelMargin : 0) // Add width/height for the margin
    Layout.preferredHeight: fixedHeight || currentLayout.implicitHeight + (isHorizontal ? 0 : 2 * panelMargin)
    Layout.fillWidth: {
        return currentLayout.children
            .filter(child => child?.applet?.plasmoid?.pluginName === "org.kde.plasma.panelspacer")
            .some(child => child.applet.plasmoid.configuration.expanding)
    }
    Layout.fillHeight: Layout.fillWidth

    property bool isHorizontal: Plasmoid.formFactor !== PlasmaCore.Types.Vertical
    property int fixedWidth: 0
    property int fixedHeight: 0
    property bool hasSpacer

    readonly property int panelMargin: 2

    Binding {
        target: root.Plasmoid
        property: "colorize"
        value: Plasmoid.configuration.colorize
    }

//END properties

//BEGIN functions
    function checkLastSpacer() {
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
    function newMenu(parent: QtObject): PlasmaExtras.Menu {
        return Qt.createQmlObject(`
        import org.kde.plasma.extras as PlasmaExtras

        PlasmaExtras.Menu {}
        `, parent) as PlasmaExtras.Menu;
    }
    function newMenuItem(parent: QtObject): PlasmaExtras.MenuItem {
        return Qt.createQmlObject(`
        import org.kde.plasma.extras as PlasmaExtras

        PlasmaExtras.MenuItem {}
        `, parent) as PlasmaExtras.MenuItem;
    }
//END functions

//BEGIN connections
    Component.onCompleted: {
        // Add our custom move action to the applet's internal actions
        Plasmoid.setInternalAction("klassikConfig", configAction);

        Plasmoid.contextualActions.push(Plasmoid.internalAction("add widgets"));
        Plasmoid.contextualActions.push(Plasmoid.internalAction("klassikConfig"));
        Plasmoid.contextualActions.push(Plasmoid.internalAction("configure"));
    }

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

        if (plasmoidItem) {
            appletsModel.remove(plasmoidItem.parent.index);
            applet["positionBeforeDeletion"] = Qt.point(plasmoidItem.parent.x, plasmoidItem.parent.y)
        }
        checkLastSpacer();
        LayoutManager.save();
    }

    Plasmoid.onUserConfiguringChanged: {
        if (Plasmoid.userConfiguring) {
            Containment.applets.forEach(applet => applet.expanded = false);
        }
    }
//END connections

//BEGIN preferences

    ConfigDialog {
        id: configDialog
    }
    PlasmaCore.Action {
        id: configAction
        text: i18n("Klassik Panel Preferences")
        icon.name: "configure"
        visible: Plasmoid.immutability !== PlasmaCore.Types.SystemImmutable
        onTriggered: {
            configDialog.show()
            configDialog.requestActivate()
        }
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

        PanelBackgroundImage {
            anchors.fill: parent
            visible: Plasmoid.configuration.useBackground
            source: Plasmoid.configuration.useCustomBackground ? Plasmoid.configuration.customBackgroundUrl
                : Qt.resolvedUrl("defaultBackground.png")
            location: Plasmoid.location
            colorize: Plasmoid.configuration.colorize
        }

        StyledFrame {
            id: panelFrame
            anchors.fill: parent
            visible: Plasmoid.configuration.drawFrame
            lineWidth: root.panelMargin
        }
    }

//END background

    DragDrop.DropArea {
        id: dropArea
        anchors.fill: parent

        Component.onCompleted: {
            LayoutManager.root = root;
            LayoutManager.layout = currentLayout;
            LayoutManager.appletsModel = appletsModel;
            LayoutManager.restore();

            root.Plasmoid.internalAction("configure").visible = Qt.binding(function() {
                return !root.Plasmoid.immutable;
            });
            root.Plasmoid.internalAction("configure").enabled = Qt.binding(function() {
                return !root.Plasmoid.immutable;
            });
        }

        onDragEnter: event => {
            if (Plasmoid.immutable) {
                event.ignore();
                return;
            }
            //during drag operations we disable panel auto resize
            root.fixedWidth = root.Layout.preferredWidth
            root.fixedHeight = root.Layout.preferredHeight
            appletsModel.insert(LayoutManager.indexAtCoordinates(event.x, event.y), {applet: dndSpacer})
        }

        onDragMove: event => {
            LayoutManager.move(dndSpacer.parent, LayoutManager.indexAtCoordinates(event.x, event.y));
        }

        onDragLeave: event => {
            const index = dndSpacer.parent.index
            if (typeof index === "number" && index > -1) {
                appletsModel.remove(index);
                root.fixedWidth = root.fixedHeight = 0;
            }
        }

        onDrop: event => {
            appletsModel.remove(dndSpacer.parent.index);
            if (!Plasmoid.processMimeData(event.mimeData, event.x, event.y)) {
                root.processMimeData(event.mimeData, event.x, event.y);
            }
            event.accept(event.proposedAction);
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
                visible: applet.Plasmoid?.status !== PlasmaCore.Types.HiddenStatus || (!Plasmoid.immutable && Plasmoid.userConfiguring) || Containment.corona.editMode;

                readonly property bool wantsToFillWidth: applet?.Layout.fillWidth
                readonly property bool wantsToFillHeight: applet?.Layout.fillHeight

                readonly property int availWidth: root.width - Layout.leftMargin - Layout.rightMargin
                readonly property int availHeight: root.height - Layout.topMargin - Layout.bottomMargin

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
                readonly property bool showHandle: applet?.Plasmoid.metaData.category !== "Application Launchers"
                                                && applet?.Plasmoid.pluginName !== "org.kde.plasma.icon"
                                                && applet?.Plasmoid.pluginName !== "com.github.neeeeow.klassik.panel.icon"
                readonly property int handleExtent: handleLoader.item?.handleExtent ?? 0

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
                    width: root.isHorizontal ? container.handleExtent ?? 0 : parent.width
                    height: root.isHorizontal ? parent.height : container.handleExtent ?? 0
                    anchors.top: parent.top
                    anchors.left: parent.left

                    HoverHandler {
                        id: hoverHandler
                    }

                    sourceComponent: GridLayout {
                        id: appletHandle
                        anchors.fill: parent
                        flow: root.isHorizontal ? GridLayout.TopToBottom : GridLayout.LeftToRight
                        rowSpacing: 0
                        columnSpacing: 0

                        // if the panel is on the bottom or the left, the handle goes first
                        readonly property bool reverse: Plasmoid.location === PlasmaCore.Types.BottomEdge || Plasmoid.location === PlasmaCore.Types.RightEdge
                        readonly property bool isMenuOpen: appletMenu.status === PlasmaExtras.Menu.Open
                        readonly property int handleExtent: grabHandle.handleExtent

                        visible: Plasmoid.configuration.showAppletHandle || hoverHandler.hovered || isMenuOpen

                        ToolButton {
                            id: menuButton
                            Layout.fillWidth: root.isHorizontal ? true : false
                            Layout.fillHeight: root.isHorizontal ? false : true
                            Layout.preferredWidth: root.isHorizontal ? -1 : height
                            Layout.preferredHeight: root.isHorizontal ? width : -1
                            Layout.row: root.isHorizontal ? (appletHandle.reverse ? 0 : 1) : 0
                            Layout.column: root.isHorizontal ? 0 : (appletHandle.reverse ? 0 : 1)

                            enabled: applet.Plasmoid.contextualActions.length > 0
                            checkable: true
                            checked: appletHandle.isMenuOpen
                            hoverEnabled: true

                            contentItem: StyledArrow {
                                sunken: menuButton.down || menuButton.checked
                                hoverEnabled: menuButton.hoverEnabled
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

                            onClicked:appletMenu.show()
                            PlasmaExtras.Menu {
                                id: appletMenu
                                function show(): void {
                                    populateMenu();
                                    open();
                                }
                                function populateMenu(): void {
                                    clearMenuItems();

                                    // Applet actions
                                    const actions = applet.Plasmoid.contextualActions;
                                    for (let i = 0; i < actions.length; ++i) {
                                        const action = actions[i];
                                        const item = newMenuItem(appletMenu);
                                        item.action = action;
                                        item.enabled = true;
                                    }

                                    // Panel menu
                                    const panelActions = Plasmoid.contextualActions;
                                    if (panelActions.length > 0) {
                                        const separator = newMenuItem(appletMenu);
                                        separator.separator = true;

                                        const panelMenuItem = newMenuItem(appletMenu);
                                        panelMenuItem.text = i18n("Panel Menu");

                                        const panelMenu = root.newMenu(panelMenuItem);
                                        for (let j = 0; j < panelActions.length; ++j) {
                                            const action = panelActions[j];
                                            const item = newMenuItem(panelMenu);
                                            item.action = action;
                                        }
                                        panelMenu.visualParent = panelMenuItem.action
                                    }
                                }
                            }
                        }

                        StyledToolBarHandle {
                            // NB: here we use a PE_IndicatorToolBarHandle here - KDE 3 used a PE_DockWindowHandle, but no such
                            // equivalent exists in Qt6.
                            id: grabHandle
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.row: root.isHorizontal ? (appletHandle.reverse ? 1 : 0) : 0
                            Layout.column: root.isHorizontal ? 0 : (appletHandle.reverse ? 1 : 0)
                            horizontal: root.isHorizontal
                            sunken: handleMouseArea.pressed
                            hoverEnabled: true

                            MouseArea {
                                id: handleMouseArea
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: pressed ? Qt.SizeAllCursor : Qt.ArrowCursor
                                onPressed: {
                                    cursorOverlay.visible = true;
                                }
                                onPositionChanged: mouse => {
                                    if (!pressed)
                                        return
                                    const globalPos = mapToItem(currentLayout, mouse.x, mouse.y);
                                    const targetIndex = LayoutManager.indexAtCoordinates(globalPos.x, globalPos.y);
                                    if (targetIndex !== container.index)
                                        LayoutManager.move(container, targetIndex);
                                }
                                onReleased: {
                                    cursorOverlay.visible = false;
                                    LayoutManager.save();
                                }
                                onCanceled: {
                                    cursorOverlay.visible = false;
                                    LayoutManager.save();
                                }
                            }
                        }
                    }
                }

                // Custom move action
                PlasmaCore.Action {
                    id: contextMenuMoveAction
                    text: i18n("Move " + applet.Plasmoid.title)
                    icon.name: "transform-move"
                    visible: Plasmoid.immutability !== PlasmaCore.Types.SystemImmutable // only allow the applet to be moved if the panel isn't immutable
                    onTriggered: globalMove.startDrag(container)
                }
                PlasmaCore.Action {
                    id: separatorAction
                    isSeparator: true
                }

                onAppletChanged: {
                    if (applet) {
                        applet.parent = container;
                        applet.anchors.fill = container;
                        applet.anchors.leftMargin = Qt.binding(() => root.isHorizontal ? container.handleExtent : 0);
                        applet.anchors.topMargin  = Qt.binding(() => root.isHorizontal ? 0 : container.handleExtent);

                        // Add our custom move action to the applet's internal actions
                        applet.Plasmoid.setInternalAction("move", contextMenuMoveAction);

                        // Add a separator to separate internal actions from other applet contextual actions
                        if (applet.Plasmoid.contextualActions.length > 0)
                            applet.Plasmoid.contextualActions.push(separatorAction);

                        // Append all required internal actions to the applet's contextual actions. This allows us to easily
                        // query the required contextual actions to appletMenu. NB: doing so makes the applet skip adding these
                        // actions as part of the normal internal action menu population, so actions do *not* get added twice.
                        applet.Plasmoid.contextualActions.push(applet.Plasmoid.internalAction("move"));
                        applet.Plasmoid.contextualActions.push(applet.Plasmoid.internalAction("remove"));
                        applet.Plasmoid.contextualActions.push(applet.Plasmoid.internalAction("configure"));
                        applet.Plasmoid.contextualActions.push(applet.Plasmoid.internalAction("alternatives"));
                    } else {
                        appletsModel.remove(index);
                    }
                }
            }
        }
//END components

//BEGIN UI elements

        anchors {
            leftMargin: root.isHorizontal ? root.panelMargin : 0
            rightMargin: root.isHorizontal ? root.panelMargin : 0
            topMargin: root.isHorizontal ? 0 : root.panelMargin
            bottomMargin: root.isHorizontal ? 0 : root.panelMargin
        }

        Item {
            id: dndSpacer
            Layout.preferredWidth: width
            Layout.preferredHeight: height
            width: root.isHorizontal ? Kirigami.Units.iconSizes.sizeForLabels * 5 : currentLayout.width
            height: root.isHorizontal ? currentLayout.height : Kirigami.Units.iconSizes.sizeForLabels * 5
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

        function startDrag(containerItem) {
            targetContainer = containerItem;
            enabled = true;
            visible = true;
        }
        function finishDrag() {
            enabled = false;
            visible = false;
            targetContainer = null;
            LayoutManager.save();
        }

        onPositionChanged: mouse => {
            if (!targetContainer)
                return;
            const targetIndex = LayoutManager.indexAtCoordinates(mouse.x, mouse.y);
            if (targetIndex !== targetContainer.index)
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
