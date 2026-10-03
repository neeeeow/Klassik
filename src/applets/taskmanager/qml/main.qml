/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-FileCopyrightText: 2012-2016 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore

import org.kde.taskmanager as TaskManager
import plasma.applet.com.github.neeeeow.klassik.taskmanager as TaskManagerApplet
import org.kde.plasma.workspace.dbus as DBus

PlasmoidItem {
    id: tasks

    // For making a bottom to top layout since qml flow can't do that.
    // We just hang the task manager upside down to achieve that.
    // This mirrors the tasks and group dialog as well, so we un-rotate them to fix that
    rotation: Plasmoid.configuration.reverseMode && Plasmoid.formFactor === PlasmaCore.Types.Vertical ? 180 : 0

    readonly property bool vertical: Plasmoid.formFactor === PlasmaCore.Types.Vertical

    // Usable task area (after subtracting frame area)
    readonly property int margin: Plasmoid.configuration.drawFrame ? sunkenFrame.lineWidth : 0
    readonly property real taskAreaWidth: tasks.width - 2*margin
    readonly property real taskAreaHeight: tasks.height - 2*margin

    readonly property Component contextMenuComponent: Qt.createComponent("ContextMenu.qml")
    readonly property Component groupMenuComponent: Qt.createComponent("GroupMenu.qml")

    property alias taskList: taskList

    preferredRepresentation: fullRepresentation

    Layout.fillWidth: true
    Layout.fillHeight: true

    signal requestLayout

    function publishIconGeometries(taskItems: /*list<Item>*/var): void {
        if (TaskManagerApplet.TaskTools.taskManagerInstanceCount >= 2) {
            return;
        }
        for (let i = 0; i < taskItems.length - 1; ++i) {
            const task = taskItems[i];

            if (!task.model.IsStartup) {
                tasksModel.requestPublishDelegateGeometry(tasksModel.makeModelIndex(task.index),
                    backend.globalRect(task), task);
            }
        }
    }

    readonly property TaskManager.TasksModel tasksModel: TaskManager.TasksModel {
        id: tasksModel

        screenGeometry: Plasmoid.containment.screenGeometry
        activity: activityInfo.currentActivity

        filterByCurrentVirtualDesktop: Plasmoid.configuration.showOnlyCurrentDesktop
        filterByScreen: Plasmoid.configuration.showOnlyCurrentScreen
        filterByActivity: Plasmoid.configuration.showOnlyCurrentActivity
        filterNotMinimized: Plasmoid.configuration.showOnlyMinimized

        sortMode: sortModeEnumValue(Plasmoid.configuration.sortingStrategy)

        groupMode: groupModeEnumValue(Plasmoid.configuration.groupingStrategy)
        groupInline: false // KDE 3 never had the option to group inline... follow that convention
        groupingWindowTasksThreshold: (Plasmoid.configuration.onlyGroupWhenFull
            ? TaskManagerApplet.LayoutMetrics.optimumCapacity(tasks.taskAreaWidth, tasks.taskAreaHeight) + 1 : -1)

        onGroupingAppIdBlacklistChanged: {
            Plasmoid.configuration.groupingAppIdBlacklist = groupingAppIdBlacklist;
        }

        function sortModeEnumValue(index: int): /*TaskManager.TasksModel.SortMode*/ int {
            switch (index) {
            case 0:
                return TaskManager.TasksModel.SortDisabled;
            // skip 1 (manual sort), since that functionality was not present in KDE 3
            case 2:
                return TaskManager.TasksModel.SortAlpha;
            case 3:
                return TaskManager.TasksModel.SortVirtualDesktop;
            case 4:
                return TaskManager.TasksModel.SortActivity;
            // 5 is SortLastActivated, skipped
            case 6:
                return TaskManager.TasksModel.SortWindowPositionHorizontal;
            default:
                return TaskManager.TasksModel.SortDisabled;
            }
        }

        function groupModeEnumValue(index: int): /*TaskManager.TasksModel.GroupMode*/ int {
            switch (index) {
            case 0:
                return TaskManager.TasksModel.GroupDisabled;
            case 1:
                return TaskManager.TasksModel.GroupApplications;
            }
        }

        Component.onCompleted: {
            groupingAppIdBlacklist = Plasmoid.configuration.groupingAppIdBlacklist;

            // Only hook up view only after the above churn is done.
            taskRepeater.model = tasksModel;
        }
    }

    readonly property TaskManagerApplet.Backend backend: TaskManagerApplet.Backend {
        id: backend
    }

    readonly property Component taskInitComponent: Component {
        Timer {
            interval: 200
            running: true

            onTriggered: {
                const task = parent as Task;
                if (task) {
                    tasks.tasksModel.requestPublishDelegateGeometry(task.modelIndex(), tasks.backend.globalRect(task), task);
                }
                destroy();
            }
        }
    }

    Connections {
        target: Plasmoid

        function onLocationChanged(): void {
            if (TaskManagerApplet.TaskTools.taskManagerInstanceCount >= 2) {
                return;
            }
            // This is on a timer because the panel may not have
            // settled into position yet when the location prop-
            // erty updates.
            iconGeometryTimer.start();
        }
    }

    Connections {
        target: Plasmoid.containment

        function onScreenGeometryChanged(): void {
            iconGeometryTimer.start();
        }
    }

    Item {
        anchors.fill: parent

        TaskManagerApplet.SunkenAppletFrame {
            id: sunkenFrame
            anchors.fill: parent
            visible: Plasmoid.configuration.drawFrame
            lineWidth: 1
        }

        TaskManager.VirtualDesktopInfo {
            id: virtualDesktopInfo
        }

        TaskManager.ActivityInfo {
            id: activityInfo
        }

        Timer {
            id: iconGeometryTimer

            interval: 500
            repeat: false

            onTriggered: {
                tasks.publishIconGeometries(taskList.children, tasks);
            }
        }

        Binding {
            target: Plasmoid
            property: "status"
            value: (tasksModel.anyTaskDemandsAttention && Plasmoid.configuration.unhideOnAttention
                ? PlasmaCore.Types.NeedsAttentionStatus : PlasmaCore.Types.PassiveStatus)
            restoreMode: Binding.RestoreBinding
        }

        Connections {
            target: Plasmoid.configuration

            function onGroupingAppIdBlacklistChanged(): void {
                tasksModel.groupingAppIdBlacklist = Plasmoid.configuration.groupingAppIdBlacklist;
            }
        }

        Component {
            id: busyIndicator
            KlassikBusyIndicator {}
        }

        MouseHandler {
            id: mouseHandler
            anchors.fill: parent
            target: taskList
        }

        TaskList {
            id: taskList
            count: tasksModel.count

            LayoutMirroring.enabled: tasks.shouldBeMirrored(Plasmoid.configuration.reverseMode, Application.layoutDirection, tasks.vertical)
            anchors {
                left: parent.left
                top: parent.top
                margins: tasks.margin
            }

            width: {
                if (vertical) {
                    return Math.min(((tasks.taskAreaWidth)/taskList.stripeCount) * count, tasks.taskAreaWidth);
                } else {
                    return Math.min(taskList.orthogonalCount * TaskManagerApplet.LayoutMetrics.preferredMaxWidth(), tasks.taskAreaWidth);
                }
            }

            height: {
                if (vertical) {
                    return Math.min(taskList.orthogonalCount * TaskManagerApplet.LayoutMetrics.preferredMaxHeight(), tasks.taskAreaHeight);
                } else {
                    return Math.min(((tasks.taskAreaHeight)/taskList.stripeCount) * count, tasks.taskAreaHeight);
                }
            }

            flow: {
                if (tasks.vertical) {
                    return Grid.LeftToRight
                }
                return Grid.TopToBottom
            }

            onAnimatingChanged: {
                if (!animating) {
                    tasks.publishIconGeometries(children, tasks);
                }
            }

            Repeater {
                id: taskRepeater

                delegate: Task {
                    tasksRoot: tasks
                }
            }
        }
    }

    // This is called by plasmashell in response to a Meta+number shortcut.
    // TODO: Change type to int
    function activateTaskAtIndex(index: var): void {
        if (typeof index !== "number") {
            return;
        }

        const task = taskRepeater.itemAt(index) as Task;
        if (task) {
            TaskManagerApplet.TaskTools.activateTask(task.modelIndex(), task.model, null, task, Plasmoid, this);
        }
    }

    function createContextMenu(rootTask, modelIndex, args = {}) {
        const initialArgs = Object.assign(args, {
            visualParent: rootTask,
            modelIndex,
            backend,
        });
        return contextMenuComponent.createObject(rootTask, initialArgs);
    }

    function createGroupMenu(rootTask, modelIndex, args = {}) {
        const initialArgs = Object.assign(args, {
            visualParent: rootTask,
            modelIndex,
        });
        return groupMenuComponent.createObject(rootTask, initialArgs);
    }

    function shouldBeMirrored(reverseMode, layoutDirection, vertical): bool {
        // LayoutMirroring is only horizontal
        if (vertical) {
            return layoutDirection === Qt.RightToLeft;
        }

        if (layoutDirection === Qt.LeftToRight) {
            return reverseMode;
        }
        return !reverseMode;
    }

    Component.onCompleted: {
        TaskManagerApplet.TaskTools.taskManagerInstanceCount += 1;
        requestLayout.connect(iconGeometryTimer.restart);
    }

    Component.onDestruction: {
        TaskManagerApplet.TaskTools.taskManagerInstanceCount -= 1;
    }
}
