/*
    SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2022 Niccolò Venerandi <niccolo@venerandi.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs
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
    Layout.preferredWidth: fixedWidth || currentLayout.implicitWidth + currentLayout.horizontalDisplacement
    Layout.preferredHeight: fixedHeight || currentLayout.implicitHeight + currentLayout.verticalDisplacement
    Layout.fillWidth: {
        return currentLayout.children
            .filter(child => child?.applet?.plasmoid?.pluginName === "org.kde.plasma.panelspacer")
            .some(child => child.applet.plasmoid.configuration.expanding)
    }
    Layout.fillHeight: Layout.fillWidth

    property Item toolBox
    property var layoutManager: LayoutManager

    property ConfigOverlay configOverlay

    property bool isHorizontal: Plasmoid.formFactor !== PlasmaCore.Types.Vertical
    property int fixedWidth: 0
    property int fixedHeight: 0
    property bool hasSpacer
    // True when e.g. the task manager is drag and dropping tasks.
    property bool appletRequestsInhibitDnD: false
    property bool reverse: Application.layoutDirection === Qt.RightToLeft

    readonly property int panelMargin: 2

//END properties

//BEGIN functions
    function checkLastSpacer() {
        for (var i = 0; i < appletsModel.count; ++i) {
            const applet = appletsModel.get(i).applet;
            if (!applet || !applet.visible || !applet.Layout || applet.isPlaceholder) {
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

    function plasmoidLocationString(): string {
        switch (Plasmoid.location) {
        case PlasmaCore.Types.LeftEdge:
            return "west";
        case PlasmaCore.Types.TopEdge:
            return "north";
        case PlasmaCore.Types.RightEdge:
            return "east";
        case PlasmaCore.Types.BottomEdge:
            return "south";
        }
        return "";
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

        if (plasmoidItem) {
            appletsModel.remove(plasmoidItem.parent.index);
            applet["positionBeforeDeletion"] = Qt.point(plasmoidItem.parent.x, plasmoidItem.parent.y)
        }
        checkLastSpacer();
        LayoutManager.save();
    }

    Plasmoid.onUserConfiguringChanged: {
        if (!Plasmoid.userConfiguring) {
            if (root.configOverlay) {
                if (root.configOverlay.dragAndDropping) {
                    root.configOverlay.finishDragOperation()
                }
                root.configOverlay.destroy();
                root.configOverlay = null;
            }
            return;
        }

        if (Plasmoid.immutable) {
            return;
        }

        Containment.applets.forEach(applet => applet.expanded = false);
        const component = Qt.createComponent("ConfigOverlay.qml");
        configOverlay = component.createObject(this, {
            "anchors.fill": dropArea,
            "anchors.rightMargin": Qt.binding(() => isHorizontal ? toolBox.height : 0),
            "anchors.bottomMargin": Qt.binding(() => !isHorizontal ? toolBox.height : 0),
            "layoutManager": Qt.binding(() => root.layoutManager),
            "appletsModel": appletsModel,
            "currentLayout": currentLayout,
            "appletContainerComponent": appletContainerComponent,
            "dropArea": dropArea,
            "isHorizontal": Qt.binding(() => root.isHorizontal),
            "rootWidth": Qt.binding(() => root.width),
            "reverse": Qt.binding(() => root.reverse),
            "lastSpacer": lastSpacer,
            "addWidgetsButton": addWidgetsButton
        });
        component.destroy();
    }
//END connections

//BEGIN preferences

    Window {
        id: configDialog
        title: i18n("Klassik Panel Preferences")
        flags: Qt.Dialog
        color: Kirigami.Theme.backgroundColor

        property var customBackgroundUrl

        function save() {
            Plasmoid.configuration.drawFrame = drawFrame.checked;
            Plasmoid.configuration.useBackground = useBackground.checked;
            Plasmoid.configuration.colorizeBackground = colorizeBackground.checked;
            Plasmoid.configuration.useCustomBackground = useCustomBackground.checked;
            Plasmoid.configuration.customBackgroundUrl = configDialog.customBackgroundUrl;
        }

        onVisibleChanged: {
            if (visible) {
                drawFrame.checked = Plasmoid.configuration.drawFrame;
                useBackground.checked = Plasmoid.configuration.useBackground;
                colorizeBackground.checked = Plasmoid.configuration.colorizeBackground;
                useCustomBackground.checked = Plasmoid.configuration.useCustomBackground;
                configDialog.customBackgroundUrl = Plasmoid.configuration.customBackgroundUrl;
            }
        }

        Kirigami.FormLayout {
            anchors.left: parent.left
            anchors.right: parent.right

            FileDialog {
                id: backgroundDialog
                title: i18n("Select panel background")
                nameFilters: ["Image files (*.bmp *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.xbm *.xpm *.svg)"]
                fileMode: FileDialog.OpenFile

                onAccepted: {
                    configDialog.customBackgroundUrl = backgroundDialog.selectedFile
                }
            }

            CheckBox {
                id: drawFrame
                text: i18n("Draw a frame around the panel")
            }
            CheckBox {
                id: useBackground
                text: i18n("Enable background image")
            }
            CheckBox {
                id: colorizeBackground
                enabled: useBackground.checked
                text: i18n("Colorize to match the desktop color scheme")
            }

            ColumnLayout {
                enabled: useBackground.checked
                RadioButton {
                    checked: !useCustomBackground.checked
                    text: i18n("Use default background")
                }
                RadioButton {
                    id: useCustomBackground
                    text: i18n("Custom background...")
                }
            }

            RowLayout {
                enabled: useBackground.checked && useCustomBackground.checked
                Label {
                    text: "Set custom panel background:"
                }

                Button {
                    id: setBackground
                    onClicked: backgroundDialog.open()
                    Layout.preferredWidth: 48
                    Layout.preferredHeight: 48
                    Layout.maximumWidth: 48
                    Layout.maximumHeight: 48

                    contentItem: Image {
                        id: backgroundPreview
                        source: configDialog.customBackgroundUrl
                        fillMode: Image.PreserveAspectFit
                    }
                }

                TextField {
                    id: imageUrl
                    readOnly: true
                    Layout.fillWidth: true
                    text: configDialog.customBackgroundUrl.toString()
                    placeholderText: i18n("No image selected...")
                }
            }
        }

        DialogButtonBox {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            standardButtons: DialogButtonBox.Ok | DialogButtonBox.Apply | DialogButtonBox.Cancel

            onAccepted: {
                configDialog.save()
                configDialog.close()
            }
            onApplied: {
                configDialog.save()
            }
            onRejected: {
                configDialog.close()
            }
        }
    }

    Plasmoid.contextualActions: [
        PlasmaCore.Action {
            text: i18n("Klassik Panel Preferences")
            icon.name: "preferences-other"
            onTriggered: {
                configDialog.show()
                configDialog.requestActivate()
            }
        }
    ]

//END preferences

//BEGIN background

    PanelBackground {
        anchors.fill: parent
        drawFrame: Plasmoid.configuration.drawFrame
        useBackground: Plasmoid.configuration.useBackground
        colorizeBackground: Plasmoid.configuration.colorizeBackground
        panelLocation: Plasmoid.location
        useCustomBackground: Plasmoid.configuration.useCustomBackground
        customBackgroundUrl: Plasmoid.configuration.customBackgroundUrl
    }

//END background

    TapHandler {
        acceptedButtons: Qt.LeftButton
        acceptedDevices: PointerDevice.TouchScreen | PointerDevice.Stylus
        onLongPressed: Plasmoid.internalAction("configure").trigger()
    }

    DragDrop.DropArea {
        id: dropArea
        anchors.fill: parent

        Component.onCompleted: {
            LayoutManager.plasmoid = root.Plasmoid;
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
            if (Plasmoid.immutable || root.appletRequestsInhibitDnD) {
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
            /*
            * When reordering task items, dragLeave signal will be emitted directly
            * without dragEnter, and in this case parent.index is undefined, so also
            * check if dndSpacer is in appletsModel.
            */
            const spacerContainer = dndSpacer.parent as AppletContainer
            if (typeof(spacerContainer?.index) === "number" && spacerContainer.index > -1) {
                appletsModel.remove(spacerContainer.index);
                root.fixedWidth = root.fixedHeight = 0;
            }
        }

        onDrop: event => {
            appletsModel.remove((dndSpacer.parent as AppletContainer).index);
            root.processMimeData(event.mimeData, event.x, event.y);
            event.accept(event.proposedAction);
            root.fixedWidth = root.fixedHeight = 0;
        }

//BEGIN components


        Component {
            id: appletContainerComponent
            // This loader conditionally manages the BusyIndicator, it's not
            // loading the applet. The applet becomes a regular child item.
            AppletContainer {
                id: container

                function getMargins(side, returnAllMargins = false, overrideFillArea = null, overrideThickArea = null): real {
                    if (!applet || !applet.Plasmoid) {
                        return 0;
                    }
                    let fillArea = overrideFillArea === null ? applet && (applet.Plasmoid.constraintHints & Plasmoid.CanFillArea) : overrideFillArea
                    var layout = {
                        top: root.isHorizontal, bottom: root.isHorizontal,
                        right: !root.isHorizontal, left: !root.isHorizontal
                    };
                    return ((layout[side] || returnAllMargins) && !fillArea) ? root.panelMargin : 0;
                }

                Layout.topMargin: getMargins('top')
                Layout.bottomMargin: getMargins('bottom')
                Layout.leftMargin: getMargins('left')
                Layout.rightMargin: getMargins('right')

                // Always fill width/height, in order to still shrink the applet when there is not enough space.
                // When the applet doesn't want to expand set a Layout.maximumWidth accordingly
                // https://bugs.kde.org/show_bug.cgi?id=473420
                Layout.fillWidth: true
                Layout.fillHeight: true

                onWantsToFillWidthChanged: root.checkLastSpacer()
                onWantsToFillHeightChanged: root.checkLastSpacer()

                availWidth: root.width - Layout.leftMargin - Layout.rightMargin
                availHeight: root.height - Layout.topMargin - Layout.bottomMargin
                function findPositive(first, second) {return first > 0 ? first : second}

    // BEGIN BUG 454095: do not combine these expressions to a function or the bindings won't work
                Layout.minimumWidth: root.isHorizontal ? findPositive(applet?.Layout.minimumWidth, availHeight) : availWidth
                Layout.minimumHeight: !root.isHorizontal ? findPositive(applet?.Layout.minimumHeight, availWidth) : availHeight

                Layout.preferredWidth: root.isHorizontal ? findPositive(applet?.Layout.preferredWidth, Layout.minimumWidth) : availWidth
                Layout.preferredHeight: !root.isHorizontal ? findPositive(applet?.Layout.preferredHeight, Layout.minimumHeight) : availHeight

                Layout.maximumWidth: root.isHorizontal ? (wantsToFillWidth ? findPositive(applet?.Layout.maximumWidth, root.width) : Math.min(applet?.Layout.maximumWidth, Layout.preferredWidth)) : availWidth
                Layout.maximumHeight: !root.isHorizontal ? (wantsToFillHeight ? findPositive(applet?.Layout.maximumHeight, root.height) : Math.min(applet?.Layout.maximumHeight, Layout.preferredHeight)) : availHeight
    // END BUG 454095

                onAppletChanged: {
                    if (applet) {
                        applet.parent = container
                        applet.anchors.fill = container
                    } else {
                        appletsModel.remove(index)
                    }
                }

                active: applet && applet.Plasmoid.busy
                sourceComponent: KlassikBusyIndicator {
                    z: 999
                }

                property int oldX: 0
                property int oldY: 0
                onXChanged: if (oldX) animateFrom(oldX, y)
                onYChanged: if (oldY) animateFrom(x, oldY)
                transform: Translate{id: translation}
                function animateFrom(xa, ya) {
                    if (root.isHorizontal) translation.x = xa - x
                    else translation.y = ya - y
                    oldX = oldY = 0
                    translAnim.running = true
                }
                NumberAnimation {
                    id: translAnim
                    duration: Kirigami.Units.shortDuration
                    easing.type: Easing.OutCubic
                    target: translation
                    properties: "x,y"
                    to: 0
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
            property bool busy: false
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

            rowSpacing: 0
            columnSpacing: 0

            x: 0
            readonly property int toolBoxSize: !root.toolBox || !Plasmoid.containment.corona.editMode ? 0 : (root.isHorizontal ? root.toolBox.width : root.toolBox.height)

            property int horizontalDisplacement: dropArea.anchors.leftMargin + dropArea.anchors.rightMargin + (root.isHorizontal ? currentLayout.toolBoxSize : 0)
            property int verticalDisplacement: dropArea.anchors.topMargin + dropArea.anchors.bottomMargin + (root.isHorizontal ? 0 : currentLayout.toolBoxSize)

    // BEGIN BUG 454095: use lastSpacer to left align applets, as implicitWidth is updated too late
            width: root.width - horizontalDisplacement
            height: root.height - verticalDisplacement

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
    MouseArea {
        anchors.fill: parent
        visible: Containment.corona.editMode && !Plasmoid.userConfiguring
        hoverEnabled: true
        onClicked: Plasmoid.internalAction("configure").trigger()
        Rectangle {
            anchors.fill: parent
            color: Kirigami.Theme.highlightColor
            opacity: 0.5
            visible: parent.containsMouse
        }
        PlasmaCore.ToolTipArea {
            id: toolTipArea
            anchors.fill: parent
            // This is to avoid the presence of mnemonics, that would
            // show the text as "&Show Panel Configuration"
            // Strip out ampersands right before non-whitespace characters, i.e.
            // those used to determine the alt key shortcut
            // (except when the word ends in ; (HTML entities))
            mainText: Plasmoid.internalAction("configure").text.replace(/(&)(?!;)\S+(?>\s)/g, "")
            icon: "configure"
        }
        Accessible.name: toolTipArea.mainText
        Accessible.description: i18ndc("plasma_shell_org.kde.plasma.desktop", "@info:whatsthis Accessible description for entering Panel edit mode click area", "Open Panel configuration ui")
        Accessible.role: Accessible.Button
    }
    ToolButton {
        id: addWidgetsButton
        anchors.centerIn: parent
        visible: appletsModel.count === 0
        text: root.isHorizontal ? i18ndc("plasma_shell_org.kde.plasma.desktop", "@action:button opens widget explorer", "Add Widgets…") : undefined
        icon.name: "list-add-symbolic"
        onClicked: Plasmoid.internalAction("add widgets").trigger()
    }
//END UI elements
}
