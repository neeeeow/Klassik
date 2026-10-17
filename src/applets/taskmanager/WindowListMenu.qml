/*
 *    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>
 *
 *    SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.extras as PlasmaExtras
import org.kde.taskmanager as TaskManager

PlasmaExtras.Menu {
    id: menu

    required property var tasksModel
    readonly property TaskManager.VirtualDesktopInfo virtualDesktopInfo: TaskManager.VirtualDesktopInfo {}
    readonly property ActionFactory actionFactory: ActionFactory {}

    placement: {
        if (Plasmoid.location === PlasmaCore.Types.LeftEdge) {
            return PlasmaExtras.Menu.RightPosedTopAlignedPopup;
        } else if (Plasmoid.location === PlasmaCore.Types.TopEdge) {
            return PlasmaExtras.Menu.BottomPosedLeftAlignedPopup;
        } else if (Plasmoid.location === PlasmaCore.Types.RightEdge) {
            return PlasmaExtras.Menu.LeftPosedTopAlignedPopup;
        } else {
            return PlasmaExtras.Menu.TopPosedLeftAlignedPopup;
        }
    }

    onStatusChanged: {
        if (status === PlasmaExtras.Menu.Closed) {
            menu.destroy();
        }
    }

    function show(): void {
        populateMenu();
        openRelative();
    }

    function newMenuItem(parent: QtObject): PlasmaExtras.MenuItem {
        return Qt.createQmlObject(`
        import org.kde.plasma.extras as PlasmaExtras

        PlasmaExtras.MenuItem {}
        `, parent) as PlasmaExtras.MenuItem;
    }

    function createTaskItem(modelIndex): PlasmaExtras.MenuItem {
        const item = newMenuItem(menu);
        const isActive = tasksModel.data(modelIndex, TaskManager.AbstractTasksModel.IsActive);

        item.text = tasksModel.data(modelIndex, Qt.DisplayRole) || "";
        item.icon = tasksModel.data(modelIndex, Qt.DecorationRole) || "";
        item.checkable = true;
        item.checked = isActive;

        item.clicked.connect(() => {
            const isMinimized = tasksModel.data(modelIndex, TaskManager.AbstractTasksModel.IsMinimized);
            if (isMinimized) {
                tasksModel.requestToggleMinimized(modelIndex);
                tasksModel.requestActivate(modelIndex);
            } else if (isActive && Plasmoid.configuration.minimizeActiveTaskOnClick) {
                tasksModel.requestToggleMinimized(modelIndex);
            } else {
                tasksModel.requestActivate(modelIndex);
            }
        });

        return item;
    }

    function populateMenu(): void {
        clearMenuItems();

        if (tasksModel.rowCount() === 0) {
            // We should never be in this scenario, since we disable the task list button if
            // the row count is 0, but we leave this here just in case.
            const emptyItem = newMenuItem(menu);
            emptyItem.enabled = false;
            emptyItem.text = i18n("No Tasks");
            return;
        }

        // IDs of each desktop
        const desktopIds = virtualDesktopInfo.desktopIds.map(String);
        // model indices of each task per destop
        const desktopTasks = desktopIds.map(() => []);

        // Iterate through each task
        for (let i = 0; i < tasksModel.rowCount(); i++) {
            const modelIndex = tasksModel.index(i, 0);
            if (tasksModel.data(modelIndex, TaskManager.AbstractTasksModel.IsStartup)) {
                continue; // ignore startup items
            }

            const onAllDesktops = tasksModel.data(modelIndex, TaskManager.AbstractTasksModel.IsOnAllVirtualDesktops);
            const taskDesktops = (tasksModel.data(modelIndex, TaskManager.AbstractTasksModel.VirtualDesktops) ?? []).map(String);

            // Check to see which groups this task belongs to, and for each
            // of these groups create a menu item and append it to the list of
            // that group's tasks
            for (let j = 0; j < desktopIds.length; j++) {
                if (onAllDesktops || taskDesktops.includes(desktopIds[j])) {
                    desktopTasks[j].push(modelIndex);
                }
            }
        }

        // For each non-empty group list, create a section and append it to the menu along with
        // the group's tasks.
        for (let i = 0; i < desktopIds.length; i++) {
            if (desktopTasks[i].length === 0) {
                continue;
            }

            const title = newMenuItem(menu);
            title.action = actionFactory.popupTitle(menu, virtualDesktopInfo.desktopNames[i], "user-desktop");

            for (const modelIndex of desktopTasks[i]) {
                addMenuItem(createTaskItem(modelIndex));
            }
        }
    }
}
