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

PlasmoidItem {
    id: tasks

    // For making a bottom to top layout since qml flow can't do that.
    // We just hang the task manager upside down to achieve that.
    // This mirrors the tasks and group dialog as well, so we un-rotate them to fix that
    rotation: Plasmoid.configuration.reverseMode && Plasmoid.formFactor === PlasmaCore.Types.Vertical ? 180 : 0

    readonly property bool vertical: Plasmoid.formFactor === PlasmaCore.Types.Vertical

    // Margin applied to the task manager
    readonly property real margin: Plasmoid.configuration.drawFrame ? sunkenFrame.lineWidth : 0

    // Width reserved for the window list button
    readonly property real windowListButtonExtent: 12

    // Total usable area for the tasks (excluding window list button!)
    readonly property real taskAreaWidth: tasks.width - 2*margin - (Plasmoid.configuration.showAllTasksMenu ? (vertical ? 0 : windowListButtonExtent) : 0)
    readonly property real taskAreaHeight: tasks.height - 2*margin - (Plasmoid.configuration.showAllTasksMenu ? (vertical ? windowListButtonExtent : 0) : 0)

    readonly property Component contextMenuComponent: Qt.createComponent("ContextMenu.qml")
    readonly property Component groupMenuComponent: Qt.createComponent("GroupMenu.qml")
    readonly property Component groupContextMenuComponent: Qt.createComponent("GroupContextMenu.qml")

    property alias taskList: taskList

    preferredRepresentation: fullRepresentation

    Layout.fillWidth: true
    Layout.fillHeight: true

    signal requestLayout

    readonly property TaskManager.TasksModel tasksModel: TaskManager.TasksModel {
        id: tasksModel

        virtualDesktop: virtualDesktopInfo.currentDesktop
        screenGeometry: Plasmoid.containment.screenGeometry
        activity: activityInfo.currentActivity

        filterByVirtualDesktop: Plasmoid.configuration.showOnlyCurrentDesktop
        filterByScreen: Plasmoid.configuration.showOnlyCurrentScreen
        filterByActivity: Plasmoid.configuration.showOnlyCurrentActivity
        filterNotMinimized: Plasmoid.configuration.showOnlyMinimized

        sortMode: sortModeEnumValue(Plasmoid.configuration.sortingStrategy)

        groupMode: groupModeEnumValue(Plasmoid.configuration.groupingStrategy)
        groupInline: false // KDE 3 never had the option to group inline... follow that convention
        groupingWindowTasksThreshold: (Plasmoid.configuration.onlyGroupWhenFull
            ? LayoutMetrics.optimumCapacity(tasks.taskAreaWidth, tasks.taskAreaHeight) + 1 : -1)

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
            default:
                return TaskManager.TasksModel.GroupDisabled;
            }
        }

        Component.onCompleted: {
            groupingAppIdBlacklist = Plasmoid.configuration.groupingAppIdBlacklist;

            // Only hook up view only after the above churn is done.
            taskRepeater.model = tasksModel;
        }
    }

    readonly property Component taskInitComponent: Component {
        Timer {
            interval: 200
            running: true

            onTriggered: {
                const task = parent as Task;
                if (task) {
                    tasks.tasksModel.requestPublishDelegateGeometry(task.modelIndex(), tasks.globalRect(task), task);
                }
                destroy();
            }
        }
    }

    Connections {
        target: Plasmoid

        function onLocationChanged(): void {
            if (TaskTools.taskManagerInstanceCount >= 2) {
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
                tasks.publishIconGeometries(taskList.children);
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

        WheelHandler {
            id: wheelHandler
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            enabled: Plasmoid.configuration.wheelEnabled !== 0

            onWheel: event => {
                // magic number 15 for common "one scroll"
                // See https://doc.qt.io/qt-6/qml-qtquick-wheelhandler.html#rotation-prop
                let increment = 0;
                while (rotation >= 15) {
                    rotation -= 15;
                    increment++;
                }
                while (rotation <= -15) {
                    rotation += 15;
                    increment--;
                }
                const p = taskList.mapFromItem(wheelHandler.parent, event.x, event.y);
                const anchor = taskList.childAt(p.x, p.y);
                while (increment !== 0) {
                    TaskTools.activateNextPrevTask(anchor, increment < 0, Plasmoid.configuration.wheelSkipMinimized, Plasmoid.configuration.wheelEnabled, tasks);
                    increment += (increment < 0) ? 1 : -1;
                }
            }
        }

        StyledFrame {
            id: sunkenFrame
            anchors.fill: parent
            visible: Plasmoid.configuration.drawFrame
            lineWidth: 1
            sunken: true
        }

        GridLayout {
            anchors.fill: parent
            anchors.margins: tasks.margin
            rowSpacing: 0
            columnSpacing: 0
            flow: tasks.vertical ? GridLayout.TopToBottom : GridLayout.LeftToRight
            LayoutMirroring.enabled: tasks.shouldBeMirrored(Plasmoid.configuration.reverseMode, Application.layoutDirection, tasks.vertical)

            Loader {
                id: windowListButtonLoader
                active: Plasmoid.configuration.showAllTasksMenu
                visible: active

                Layout.fillWidth: tasks.vertical ? true : false
                Layout.fillHeight: tasks.vertical ? false : true
                Layout.preferredWidth: tasks.vertical ? -1 : tasks.windowListButtonExtent
                Layout.preferredHeight: tasks.vertical ? tasks.windowListButtonExtent : -1

                sourceComponent: WindowListButton {}
            }

            GridLayout {
                id: taskList

                property int count: tasksModel.count
                readonly property int stripeCount: LayoutMetrics.stripeCount()
                readonly property int orthogonalCount: LayoutMetrics.orthogonalCount(count)

                rowSpacing: 0
                columnSpacing: 0
                rows: tasks.vertical ? orthogonalCount : stripeCount
                columns: tasks.vertical ? stripeCount : orthogonalCount
                flow: tasks.vertical ? Grid.LeftToRight : Grid.TopToBottom

                LayoutMirroring.enabled: tasks.shouldBeMirrored(Plasmoid.configuration.reverseMode, Application.layoutDirection, tasks.vertical)
                Layout.alignment: Qt.AlignLeft | Qt.AlignTop
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.maximumWidth: tasks.vertical ? ((tasks.taskAreaWidth / stripeCount) * count) : (orthogonalCount * LayoutMetrics.preferredMaxWidth())
                Layout.maximumHeight: tasks.vertical ? (orthogonalCount * LayoutMetrics.preferredMaxHeight()) : ((tasks.taskAreaHeight / stripeCount) * count)

                Repeater {
                    id: taskRepeater

                    delegate: Task {
                        tasksRoot: tasks
                    }
                }
            }

            Item {
                // this item simply fills out empty space in our gridlayout
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }

    function publishIconGeometries(taskItems: /*list<Item>*/var): void {
        if (TaskTools.taskManagerInstanceCount >= 2) {
            return;
        }
        for (let i = 0; i < taskItems.length - 1; ++i) {
            const task = taskItems[i];

            if (!task.model.IsStartup) {
                tasksModel.requestPublishDelegateGeometry(tasksModel.makeModelIndex(task.index),
                                                          tasks.globalRect(task), task);
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
            TaskTools.activateTask(task.modelIndex(), task.model, task, Plasmoid, this);
        }
    }

    function createContextMenu(rootTask, modelIndex) {
        const isGroup = tasksModel.data(modelIndex, TaskManager.AbstractTasksModel.IsGroupParent);
        const component = isGroup ? groupContextMenuComponent : contextMenuComponent;
        return component.createObject(rootTask, {
            visualParent: rootTask,
            modelIndex
        });
    }

    function createGroupMenu(rootTask, modelIndex) {
        return groupMenuComponent.createObject(rootTask, {
            visualParent: rootTask,
            modelIndex,
        });
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

    function globalRect(item: Item): rect {
        if (!item || !item.Window.window) {
            return Qt.rect(0, 0, 0, 0);
        }
        const iconCoords = item.mapToGlobal(0, 0);
        // apply rounding to follow the original C++ code which returns a QRect.
        return Qt.rect(Math.round(iconCoords.x), Math.round(iconCoords.y), Math.round(item.width), Math.round(item.height));
    }

    Component.onCompleted: {
        TaskTools.taskManagerInstanceCount += 1;
        requestLayout.connect(iconGeometryTimer.restart);
    }

    Component.onDestruction: {
        TaskTools.taskManagerInstanceCount -= 1;
    }
}
