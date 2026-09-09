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
import plasma.applet.com.github.neeeeow.klassik.taskmanager as TaskManagerApplet

PlasmaExtras.Menu {
    id: groupMenu

    required property /*QModelIndex*/var modelIndex


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

    minimumWidth: (visualParent as Item).width

    onStatusChanged: {
        if (status === PlasmaExtras.Menu.Closed) {
            groupMenu.destroy();
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

    function populateMenu(): void {
        clearMenuItems();

        const root = visualParent as Task;
        if (!root)
            return;

        const tasksModel = root.tasksRoot.tasksModel;

        TaskManagerApplet.TaskTools.foreachChildTask((childIndex) => {
            const item = newMenuItem(groupMenu);
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
                } else if (isActive && plasmoid.configuration.minimizeActiveTaskOnClick) {
                    tasksModel.requestToggleMinimized(childIndex);
                } else {
                    tasksModel.requestActivate(childIndex);
                }
            });

            addMenuItem(item);
        }, modelIndex, tasksModel);
    }
}
