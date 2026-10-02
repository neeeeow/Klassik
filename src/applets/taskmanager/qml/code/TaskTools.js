/*
    SPDX-FileCopyrightText: 2012-2016 Eike Hein <hein@kde.org>
    SPDX-FileCopyrightText: 2020 Nate Graham <nate@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

.pragma library

.import org.kde.taskmanager as TaskManager
.import org.kde.plasma.core as PlasmaCore // Needed by TaskManager

// Can't be `let`, or else QML counterpart won't be able to assign to it.
var taskManagerInstanceCount = 0;

function activateNextPrevTask(anchor, next, wheelSkipMinimized, wheelEnabled, tasks) {
    // FIXME TODO: Unnecessarily convoluted and costly; optimize.

    if (wheelEnabled === 2) { // TaskOnly
        // anchor can be undefined if scrolling on an empty area
        if (!anchor) return;
        // Get the number of open windows for the application
        const anchorModelIndex = anchor.modelIndex(anchor.index);
        const winIdList = tasks.tasksModel.data(anchorModelIndex, TaskManager.AbstractTasksModel.WinIdList);
        const windowCount = winIdList ? winIdList.length : 0;
        // No windows
        if (windowCount === 0) {
            return;
        }

        // Single window
        if (windowCount === 1) {
            const isHidden = tasks.tasksModel.data(anchorModelIndex, TaskManager.AbstractTasksModel.IsHidden);
            if (!wheelSkipMinimized || !isHidden) {
                tasks.tasksModel.requestActivate(anchorModelIndex);
            }
            return;
        }
    }

    // Multiple windows
    let taskIndexList = [];
    const activeTaskIndex = tasks.tasksModel.activeTask;

    for (let i = 0; i < tasks.taskList.children.length - 1; ++i) {
        const task = tasks.taskList.children[i];
        const modelIndex = task.modelIndex(i);

        if (!task.model.IsStartup) {
            if (task.model.IsGroupParent) {
                if (wheelEnabled === 2 && task === anchor) { // If TaskOnly mode and the anchor is a group parent, collect only windows within the group.
                    taskIndexList = [];
                }

                for (let j = 0; j < tasks.tasksModel.rowCount(modelIndex); ++j) {
                    const childModelIndex = tasks.tasksModel.makeModelIndex(i, j);
                    const childHidden = tasks.tasksModel.data(childModelIndex, TaskManager.AbstractTasksModel.IsHidden);
                    if (!wheelSkipMinimized || !childHidden) {
                        taskIndexList.push(childModelIndex);
                    }
                }

                if (wheelEnabled === 2 && task === anchor) { // If TaskOnly mode, break after processing the anchor group.
                    break;
                }
            } else {
                if (!wheelSkipMinimized || !task.model.IsHidden) {
                    taskIndexList.push(modelIndex);
                }
            }
        }
    }

    if (!taskIndexList.length) {
        return;
    }

    let target = taskIndexList[0];

    for (let i = 0; i < taskIndexList.length; ++i) {
        if (taskIndexList[i] === activeTaskIndex)
        {
            if (next && i < (taskIndexList.length - 1)) {
                target = taskIndexList[i + 1];
            } else if (!next) {
                if (i) {
                    target = taskIndexList[i - 1];
                } else {
                    target = taskIndexList[taskIndexList.length - 1];
                }
            }

            break;
        }
    }

    tasks.tasksModel.requestActivate(target);
}

function activateTask(index, model, modifiers, task, plasmoid, tasks) {
    if (modifiers & Qt.ShiftModifier) {
        tasks.tasksModel.requestNewInstance(index);
        return;
    }
    // Publish delegate geometry again if there are more than one task manager instance
    if (taskManagerInstanceCount >= 2) {
        tasks.tasksModel.requestPublishDelegateGeometry(task.modelIndex(), tasks.backend.globalRect(task), task);
    }

    if (model.IsGroupParent) {
        // Open group menu
        task.showGroupMenu();
    } else {
        if (model.IsMinimized) {
            tasks.tasksModel.requestToggleMinimized(index);
            tasks.tasksModel.requestActivate(index);
        } else if (model.IsActive && plasmoid.configuration.minimizeActiveTaskOnClick) {
            tasks.tasksModel.requestToggleMinimized(index);
        } else {
            tasks.tasksModel.requestActivate(index);
        }
    }
}

function foreachChildTask(callback, modelIndex, tasksModel) {
    const childCount = tasksModel.rowCount(modelIndex);
    if (childCount > 0) {
        for (let i = 0; i < childCount; ++i) {
            const childIndex = tasksModel.index(i, 0, modelIndex);
            callback(childIndex);
        }
    } else {
        callback(modelIndex);
    }
}
