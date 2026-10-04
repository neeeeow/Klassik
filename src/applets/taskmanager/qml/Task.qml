/*
 SPDX-FileCopyrightText: 2026 neeeeow *<https://github.com/neeeeow>

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

Button {
    id: task

    // To achieve a bottom-to-top layout on vertical panels, the task manager
    // is rotated by 180 degrees(see main.qml). This makes the tasks rotated,
    // so un-rotate them here to fix that.
    rotation: Plasmoid.configuration.reverseMode && Plasmoid.formFactor === PlasmaCore.Types.Vertical ? 180 : 0

    Layout.fillWidth: true
    Layout.fillHeight: true
    Layout.maximumWidth: tasksRoot.vertical ? -1 : TaskManagerApplet.LayoutMetrics.preferredMaxWidth()
    Layout.maximumHeight: tasksRoot.vertical ? TaskManagerApplet.LayoutMetrics.preferredMaxHeight() : -1

    leftPadding: 4
    rightPadding: 4
    topPadding: 0
    bottomPadding: 0

    focusPolicy: Qt.NoFocus
    checked: task.model.IsActive

    required property var model
    required property int index
    required property /*main.qml*/ Item tasksRoot

    property bool isWindow: model.IsWindow
    property int childCount: model.ChildCount
    property int previousChildCount: 0
    property QtObject contextMenu: null
    property QtObject groupMenu: null

    property bool completed: false

    function modelIndex(): /*QModelIndex*/ var {
        return tasksModel.makeModelIndex(index);
    }

    function showContextMenu(): void {
        toolTipArea.hideImmediately();
        contextMenu = tasksRoot.createContextMenu(task, modelIndex()) as PlasmaExtras.Menu;
        contextMenu.show();
    }

    function showGroupMenu(): void {
        toolTipArea.hideImmediately();
        groupMenu = tasksRoot.createGroupMenu(task, modelIndex()) as TaskManagerApplet.GroupMenu;
        groupMenu.show();
    }

    onXChanged: if (completed) tasksRoot.requestLayout()
    onYChanged: if (completed) tasksRoot.requestLayout()

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
        toolTipArea.hideToolTip();

        if (!tasksRoot.vertical) {
            tasksRoot.requestLayout();
        }
    }

    onClicked: {
        if (toolTipArea.active) {
            toolTipArea.hideToolTip();
        }
        TaskManagerApplet.TaskTools.activateTask(modelIndex(), model, task, Plasmoid, tasksRoot);
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton | Qt.MiddleButton

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

    PlasmaCore.ToolTipArea {
        id: toolTipArea
        anchors.fill: parent

        active: task.groupMenu?.status !== PlasmaExtras.Menu.Open && task.contextMenu?.status !== PlasmaExtras.Menu.Open
        location: Plasmoid.location
        mainText: task.model.display
        icon: task.model.decoration
    }

    background: TaskBackground {
        visible: (Plasmoid.configuration.taskAppearance === 1) || task.hovered
        sunken: task.down || task.checked
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
                anchors.fill: parent
                active: task.model.IsStartup
                sourceComponent: busyIndicator
            }
        }

        Label {
            id: label

            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter

            visible: (task.width - iconBox.width - Kirigami.Units.smallSpacing) >= TaskManagerApplet.LayoutMetrics.spaceRequiredToShowText()

            text: task.model.display
            elide: Text.ElideRight
            textFormat: Text.PlainText
            verticalAlignment: Text.AlignVCenter
            maximumLineCount: 1
            font.bold: task.model.IsActive
            opacity: task.model.IsMinimized ? 0.5 : 1.0
        }
    }

    Component.onCompleted: {
        if (!model.IsWindow) {
            taskInitComponent.createObject(task);
        }
        completed = true;
    }
}
