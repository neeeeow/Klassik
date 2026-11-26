/*
    SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
 */
pragma ComponentBehavior: Bound

import QtQuick
import org.kde.plasma.plasmoid
import org.kde.plasma.extras as PlasmaExtras
import org.kde.taskmanager as TaskManager

PlasmaExtras.Menu {
    id: menu

    required property /*main.qml*/ Item tasksRoot
    required property /*QModelIndex*/var modelIndex

    placement: tasksRoot.menuPlacement

    minimumWidth: (visualParent as Item)?.width ?? 0

    onStatusChanged: {
        if (status === PlasmaExtras.Menu.Closed) {
            menu.destroy();
        }
    }

    function show(): void {
        populateMenu();
        openRelative();

        if (status === PlasmaExtras.Menu.Closed)
            menu.destroy();
    }

    function populateMenu(): void {
        clearMenuItems();

        const tasksModel = tasksRoot.tasksModel;
        TaskTools.foreachChildTask((childIndex) => {
            const item = tasksRoot.newMenuItem(menu);
            const isActive = tasksModel.data(childIndex, TaskManager.AbstractTasksModel.IsActive);

            item.text = tasksModel.data(childIndex, Qt.DisplayRole) || "";
            item.icon = tasksModel.data(childIndex, Qt.DecorationRole) || "";
            item.checkable = true;
            item.checked = isActive;

            item.clicked.connect(() => {
                const isMinimized = tasksModel.data(childIndex, TaskManager.AbstractTasksModel.IsMinimized);
                if (isMinimized) {
                    tasksModel.requestToggleMinimized(childIndex);
                    tasksModel.requestActivate(childIndex);
                } else if (isActive && Plasmoid.configuration.minimizeActiveTaskOnClick) {
                    tasksModel.requestToggleMinimized(childIndex);
                } else {
                    tasksModel.requestActivate(childIndex);
                }
            });
        }, menu.modelIndex, tasksModel);
    }
}
