/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
pragma ComponentBehavior: Bound

import QtQuick
import org.kde.plasma.plasmoid
import org.kde.plasma.extras as PlasmaExtras
import org.kde.taskmanager as TaskManager

import com.github.neeeeow.klassik.actionfactory

PlasmaExtras.Menu {
    id: menu

    required property /*main.qml*/ Item tasksRoot
    required property TaskManager.TasksModel tasksModel

    placement: tasksRoot.menuPlacement

    function show(): void {
        populateMenu();
        openRelative();
    }

    function createTaskItem(modelIndex: /*QModelIndex*/ var): void {
        const item = tasksRoot.newMenuItem(menu);
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
    }

    function populateMenu(): void {
        clearMenuItems();

        const virtualDesktopInfo = tasksRoot.virtualDesktopInfo;
        // IDs of each desktop
        const desktopIds = virtualDesktopInfo.desktopIds.map(String);
        // model indices of each task per desktop
        const desktopTasks = desktopIds.map(() => []);

        // Iterate through each task
        for (let i = 0; i < tasksModel.rowCount(); ++i) {
            const modelIndex = tasksModel.index(i, 0);
            if (tasksModel.data(modelIndex, TaskManager.AbstractTasksModel.IsStartup)) {
                continue; // ignore startup items
            }

            const onAllDesktops = tasksModel.data(modelIndex, TaskManager.AbstractTasksModel.IsOnAllVirtualDesktops);
            const taskDesktops = (tasksModel.data(modelIndex, TaskManager.AbstractTasksModel.VirtualDesktops) ?? []).map(String);

            // Check to see which groups this task belongs to, and for each
            // of these groups create a menu item and append it to the list of
            // that group's tasks
            for (let j = 0; j < desktopIds.length; ++j) {
                if (onAllDesktops || taskDesktops.includes(desktopIds[j])) {
                    desktopTasks[j].push(modelIndex);
                }
            }
        }

        // For each non-empty group list, create a section and append it to the menu along with
        // the group's tasks.
        for (let i = 0; i < desktopIds.length; ++i) {
            if (desktopTasks[i].length === 0) {
                continue;
            }

            const title = tasksRoot.newMenuItem(menu);
            title.action = ActionFactory.popupTitle("user-desktop", virtualDesktopInfo.desktopNames[i], title);

            for (const modelIndex of desktopTasks[i]) {
                createTaskItem(modelIndex);
            }
        }

        // If the menu is empty...
        if (menu.content.length === 0) {
            const emptyItem = tasksRoot.newMenuItem(menu);
            emptyItem.enabled = false;
            emptyItem.text = i18n("No Tasks");
        }
    }
}
