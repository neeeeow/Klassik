/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-FileCopyrightText: 2012-2013 Eike Hein <hein@kde.org>
    SPDX-FileCopyrightText: 2024 Nate Graham <nate@kde.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.extras as PlasmaExtras
import org.kde.kirigami as Kirigami
import plasma.applet.com.github.neeeeow.klassik.taskmanager as TaskManagerApplet
import org.kde.plasma.plasmoid

import org.kde.taskmanager as TaskManager

PlasmaCore.ToolTipArea {
    id: task

    activeFocusOnTab: true

    // To achieve a bottom-to-top layout on vertical panels, the task manager
    // is rotated by 180 degrees(see main.qml). This makes the tasks rotated,
    // so un-rotate them here to fix that.
    rotation: Plasmoid.configuration.reverseMode && Plasmoid.formFactor === PlasmaCore.Types.Vertical ? 180 : 0

    Layout.fillWidth: true
    Layout.fillHeight: true
    Layout.maximumWidth: tasksRoot.vertical ? -1 : TaskManagerApplet.LayoutMetrics.preferredMaxWidth()
    Layout.maximumHeight: tasksRoot.vertical ? TaskManagerApplet.LayoutMetrics.preferredMaxHeight() : -1

    required property var model
    required property int index
    required property /*main.qml*/ Item tasksRoot

    property bool isWindow: model.IsWindow
    property int childCount: model.ChildCount
    property int previousChildCount: 0
    property QtObject contextMenu: null
    property QtObject groupMenu: null

    property bool completed: false

    active: task.groupMenu?.status !== PlasmaExtras.Menu.Open && task.contextMenu?.status !== PlasmaExtras.Menu.Open
    location: Plasmoid.location
    mainText: task.model.display
    icon: task.model.decoration

    onXChanged: {
        if (!completed) {
            return;
        }
        if (oldX < 0) {
            oldX = x;
            return;
        }
        moveAnim.x = oldX - x + translateTransform.x;
        moveAnim.y = translateTransform.y;
        oldX = x;
        moveAnim.restart();
    }
    onYChanged: {
        if (!completed) {
            return;
        }
        if (oldY < 0) {
            oldY = y;
            return;
        }
        moveAnim.y = oldY - y + translateTransform.y;
        moveAnim.x = translateTransform.x;
        oldY = y;
        moveAnim.restart();
    }

    property real oldX: -1
    property real oldY: -1
    SequentialAnimation {
        id: moveAnim
        property real x
        property real y
        onRunningChanged: {
            if (running) {
                ++task.parent.animationsRunning;
            } else {
                --task.parent.animationsRunning;
            }
        }
        ParallelAnimation {
            NumberAnimation {
                target: translateTransform
                properties: "x"
                from: moveAnim.x
                to: 0
                easing.type: Easing.OutQuad
                duration: Kirigami.Units.longDuration
            }
            NumberAnimation {
                target: translateTransform
                properties: "y"
                from: moveAnim.y
                to: 0
                easing.type: Easing.OutQuad
                duration: Kirigami.Units.longDuration
            }
        }
    }
    transform: Translate {
        id: translateTransform
    }

    onContainsMouseChanged: {
        if (containsMouse) {
            task.forceActiveFocus(Qt.MouseFocusReason);
        }
    }

    onIsWindowChanged: {
        if (model.IsWindow) {
            taskInitComponent.createObject(task);
        }
    }

    onChildCountChanged: {
        if (TaskManagerApplet.TaskTools.taskManagerInstanceCount < 2 && childCount > previousChildCount) {
            tasksModel.requestPublishDelegateGeometry(modelIndex(), backend.globalRect(task), task);
        }

        previousChildCount = childCount;
    }

    onIndexChanged: {
        hideToolTip();

        if (!tasksRoot.vertical) {
            tasksRoot.requestLayout();
        }
    }

    Keys.onMenuPressed: event => contextMenuTimer.start()
    Keys.onReturnPressed: event => TaskManagerApplet.TaskTools.activateTask(modelIndex(), model, event.modifiers, task, Plasmoid, tasksRoot)
    Keys.onEnterPressed: event => Keys.returnPressed(event);
    Keys.onSpacePressed: event => Keys.returnPressed(event);
    Keys.onUpPressed: event => Keys.leftPressed(event)
    Keys.onDownPressed: event => Keys.rightPressed(event)
    Keys.onLeftPressed: event => {
        if ((event.modifiers & Qt.ControlModifier) && (event.modifiers & Qt.ShiftModifier)) {
            tasksModel.move(task.index, task.index - 1);
        } else {
            event.accepted = false;
        }
    }
    Keys.onRightPressed: event => {
        if ((event.modifiers & Qt.ControlModifier) && (event.modifiers & Qt.ShiftModifier)) {
            tasksModel.move(task.index, task.index + 1);
        } else {
            event.accepted = false;
        }
    }

    function modelIndex(): /*QModelIndex*/ var {
        return tasksModel.makeModelIndex(index);
    }

    function showContextMenu(args: var): void {
        task.hideImmediately();
        contextMenu = tasksRoot.createContextMenu(task, modelIndex(), args) as TaskManagerApplet.ContextMenu;
        contextMenu.show();
    }

    function showGroupMenu(args: var): void {
        task.hideImmediately();
        groupMenu = tasksRoot.createGroupMenu(task, modelIndex(), args) as TaskManagerApplet.GroupMenu;
        groupMenu.show();
    }

    Button {
        id: frame
        leftPadding: 4
        rightPadding: 4
        topPadding: 2
        bottomPadding: 2

        anchors.fill: parent

        focusPolicy: Qt.NoFocus
        checked: task.model.IsActive

        onClicked: { // logic from leftTapHandler
            if (dragHandler.dragTriggered)
                return;
            if (task.active) {
                task.hideToolTip();
            }
            TaskManagerApplet.TaskTools.activateTask(modelIndex(), model, Qt.NoModifier, task, Plasmoid, tasksRoot);
        }

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton | Qt.MiddleButton
            propagateComposedEvents: true

            onClicked: (mouse) => {
                if (mouse.button === Qt.RightButton) {
                    task.showContextMenu();
                } else if (mouse.button === Qt.MiddleButton) {
                    if (Plasmoid.configuration.middleClickAction === TaskManagerApplet.Backend.NewInstance) {
                        tasksModel.requestNewInstance(modelIndex());
                    } else if (Plasmoid.configuration.middleClickAction === TaskManagerApplet.Backend.Close) {
                        tasksModel.requestClose(modelIndex());
                    } else if (Plasmoid.configuration.middleClickAction === TaskManagerApplet.Backend.ToggleMinimized) {
                        tasksModel.requestToggleMinimized(modelIndex());
                    } else if (Plasmoid.configuration.middleClickAction === TaskManagerApplet.Backend.ToggleGrouping) {
                        tasksModel.requestToggleGrouping(modelIndex());
                    } else if (Plasmoid.configuration.middleClickAction === TaskManagerApplet.Backend.BringToCurrentDesktop) {
                        TaskManagerApplet.TaskTools.foreachChildTask((childIndex) => {
                            tasksModel.requestVirtualDesktops(childIndex, [virtualDesktopInfo.currentDesktopByScreenGeometry(tasksModel.data(childIndex, TaskManager.AbstractTasksModel.ScreenGeometry))]);
                        }, modelIndex(), tasksModel);
                    }
                }
            }
        }

        background: TaskBackground {
            visible: (Plasmoid.configuration.taskAppearance === 1) || frame.hovered
            sunken: frame.down || frame.checked
        }

        // Avoid repositioning delegate item after dragFinished
        DragHandler {
            id: dragHandler
            grabPermissions: PointerHandler.CanTakeOverFromItems | PointerHandler.CanTakeOverFromHandlersOfDifferentType
            property bool dragTriggered: false

            function setRequestedInhibitDnd(value: bool): void {
                // This is modifying the value in the panel containment that
                // inhibits accepting drag and drop, so that we don't accidentally
                // drop the task on this panel.
                let item = this;
                while (item.parent) {
                    item = item.parent;
                    if (item.appletRequestsInhibitDnD !== undefined) {
                        item.appletRequestsInhibitDnD = value
                    }
                }
            }

            onActiveChanged: {
                if (active) {
                    dragTriggered = true
                    icon.grabToImage(result => {
                        if (!dragHandler.active) {
                            // BUG 466675 grabToImage is async, so avoid updating dragSource when active is false
                            return;
                        }
                        setRequestedInhibitDnd(true);
                        tasksRoot.dragSource = task;
                        dragHelper.Drag.imageSource = result.url;
                        dragHelper.Drag.mimeData = {
                            "text/x-orgkdeplasmataskmanager_taskurl": backend.tryDecodeApplicationsUrl(model.LauncherUrlWithoutIcon).toString(),
                                     [model.MimeType]: model.MimeData,
                                     "application/x-orgkdeplasmataskmanager_taskbuttonitem": model.MimeData,
                        };
                        dragHelper.Drag.active = dragHandler.active;
                    });
                } else {
                    setRequestedInhibitDnd(false);
                    dragHelper.Drag.active = false;
                    dragHelper.Drag.imageSource = "";
                    if (dragTriggered) {
                        frame.checked = Qt.binding(() => model.IsActive);
                        dragTriggered = false;
                    }
                }
            }
        }

        contentItem: RowLayout {
            spacing: Kirigami.Units.smallSpacing

            Item {
                id: iconBox
                Layout.preferredHeight: Kirigami.Units.iconSizes.small
                Layout.preferredWidth: Layout.preferredHeight

                Kirigami.Icon {
                    id: icon
                    anchors.fill: parent
                    source: task.model.decoration
                    opacity: task.model.IsMinimized ? 0.5 : 1.0
                }

                Loader {
                    anchors.centerIn: parent
                    width: parent.width
                    height: parent.height
                    active: task.model.IsStartup
                    sourceComponent: busyIndicator
                }
            }

            Label {
                id: label

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter

                visible: (frame.width - iconBox.width - Kirigami.Units.smallSpacing) >= TaskManagerApplet.LayoutMetrics.spaceRequiredToShowText()

                text: task.model.display
                elide: Text.ElideRight
                textFormat: Text.PlainText
                verticalAlignment: Text.AlignVCenter
                maximumLineCount: 1
                font.bold: task.model.IsActive
                opacity: model.IsMinimized ? 0.5 : 1.0
            }
        }
    }

    Component.onCompleted: {
        if (!model.IsWindow) {
            taskInitComponent.createObject(task);
        }
        completed = true;
    }
    Component.onDestruction: {
        if (moveAnim.running) {
            (task.parent as TaskList).animationsRunning -= 1;
        }
    }
}
