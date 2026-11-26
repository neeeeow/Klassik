/*
      SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

      SPDX-License-Identifier: GPL-3.0-or-later
 */
pragma ComponentBehavior: Bound

import QtQuick

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

    function get(modelProp: int): var {
        return tasksRoot.tasksModel.data(modelIndex, modelProp);
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
        const virtualDesktopInfo = tasksRoot.virtualDesktopInfo;
        const activityInfo = tasksRoot.activityInfo;

        // Add per-task controls
        TaskTools.foreachChildTask((childIndex) => {
            const item = tasksRoot.newMenuItem(menu);

            item.text = tasksModel.data(childIndex, Qt.DisplayRole) || "";
            item.icon = tasksModel.data(childIndex, Qt.DecorationRole) || "";

            tasksRoot.contextMenuComponent.createObject(item, {
                tasksRoot: menu.tasksRoot,
                visualParent: item.action,
                modelIndex: childIndex
            });
        }, menu.modelIndex, tasksModel);

        tasksRoot.newSeparator(menu);

        // Add desktops submenu
        if (virtualDesktopInfo.numberOfDesktops > 1 && menu.get(TaskManager.AbstractTasksModel.IsVirtualDesktopsChangeable)) {
            const taskDesktops = menu.get(TaskManager.AbstractTasksModel.VirtualDesktops) ?? [];

            const virtualDesktopsMenuItem = tasksRoot.newMenuItem(menu);
            virtualDesktopsMenuItem.text = i18n("All to &Desktop");
            virtualDesktopsMenuItem.icon = "virtual-desktops";

            const virtualDesktopsMenu = tasksRoot.newMenu(virtualDesktopsMenuItem);
            virtualDesktopsMenu.visualParent = virtualDesktopsMenuItem.action;

            let menuItem = tasksRoot.newMenuItem(virtualDesktopsMenu);
            menuItem.text = i18n("All &to Current Desktop");
            let isAnyTaskNotOnCurrentDesktop = false;
            TaskTools.foreachChildTask((childIndex) => {
                const screenGeometry = tasksModel.data(childIndex, TaskManager.AbstractTasksModel.ScreenGeometry);
                const currentDesktop = virtualDesktopInfo.currentDesktopByScreenGeometry(screenGeometry);
                const childDesktops = tasksModel.data(childIndex, TaskManager.AbstractTasksModel.VirtualDesktops) ?? [];
                if (childDesktops.indexOf(currentDesktop) === -1) {
                    isAnyTaskNotOnCurrentDesktop = true;
                }
            }, menu.modelIndex, tasksModel);
            menuItem.enabled = Boolean(menu.visualParent && isAnyTaskNotOnCurrentDesktop);
            menuItem.clicked.connect(() => {
                TaskTools.foreachChildTask((childIndex) => {
                    tasksModel.requestVirtualDesktops(childIndex, [virtualDesktopInfo.currentDesktopByScreenGeometry(tasksModel.data(childIndex, TaskManager.AbstractTasksModel.ScreenGeometry))]);
                }, menu.modelIndex, tasksModel);
            });

            menuItem = tasksRoot.newMenuItem(virtualDesktopsMenu);
            menuItem.text = i18n("&All Desktops");
            menuItem.checkable = true;
            menuItem.checked = Boolean(menu.visualParent && menu.get(TaskManager.AbstractTasksModel.IsOnAllVirtualDesktops));
            menuItem.clicked.connect(() => {
                tasksModel.requestVirtualDesktops(menu.modelIndex, []);
            });

            tasksRoot.newSeparator(virtualDesktopsMenu);

            for (let i = 0; i < virtualDesktopInfo.desktopNames.length; ++i) {
                const desktopId = virtualDesktopInfo.desktopIds[i];

                menuItem = tasksRoot.newMenuItem(virtualDesktopsMenu);
                menuItem.text = virtualDesktopInfo.desktopNames[i];
                menuItem.checkable = true;
                menuItem.checked = Boolean(menu.visualParent && taskDesktops.indexOf(desktopId) > -1);
                menuItem.clicked.connect(() => {
                    tasksModel.requestVirtualDesktops(menu.modelIndex, [desktopId]);
                });
            }
        }

        // Add activities sub menu
        if (activityInfo.numberOfRunningActivities > 1) {
            const onActivities = menu.get(TaskManager.AbstractTasksModel.Activities) ?? [];

            const activitiesDesktopsMenuItem = tasksRoot.newMenuItem(menu);
            activitiesDesktopsMenuItem.text = i18n("Show All in &Activities");
            activitiesDesktopsMenuItem.icon = "activities";

            const activitiesDesktopsMenu = tasksRoot.newMenu(activitiesDesktopsMenuItem);
            activitiesDesktopsMenu.visualParent = activitiesDesktopsMenuItem.action;

            let menuItem = tasksRoot.newMenuItem(activitiesDesktopsMenu);
            menuItem.text = i18n("Add All to Current Activity");
            menuItem.enabled = Boolean(menu.visualParent && onActivities.length > 0 &&
                onActivities.indexOf(activityInfo.currentActivity) < 0);
            menuItem.clicked.connect(() => {
                tasksModel.requestActivities(menu.modelIndex, onActivities.concat(activityInfo.currentActivity));
            });

            menuItem = tasksRoot.newMenuItem(activitiesDesktopsMenu);
            menuItem.text = i18n("All Activities");
            menuItem.checkable = true;
            menuItem.checked = Boolean(menu.visualParent && onActivities.length === 0);
            menuItem.toggled.connect(checked => {
                let newActivities = []; // will cast to an empty QStringList i.e all activities
                if (!checked) {
                    newActivities = [activityInfo.currentActivity];
                }
                tasksModel.requestActivities(menu.modelIndex, newActivities);
            });

            tasksRoot.newSeparator(activitiesDesktopsMenu);

            const runningActivities = activityInfo.runningActivities();
            for (let i = 0; i < runningActivities.length; ++i) {
                const activityId = runningActivities[i];

                menuItem = tasksRoot.newMenuItem(activitiesDesktopsMenu);
                menuItem.text = activityInfo.activityName(activityId);
                menuItem.icon = activityInfo.activityIcon(activityId);
                menuItem.checkable = true;
                menuItem.checked = Boolean(menu.visualParent && onActivities.indexOf(activityId) >= 0);
                menuItem.toggled.connect(checked => {
                    let newActivities = [...(menu.get(TaskManager.AbstractTasksModel.Activities) ?? [])];
                    if (checked) {
                        if (newActivities.indexOf(activityId) < 0)
                            newActivities.push(activityId);
                    } else {
                        const index = newActivities.indexOf(activityId);
                        if (index < 0) {
                            return;
                        }

                        newActivities.splice(index, 1);
                    }
                    tasksModel.requestActivities(menu.modelIndex, newActivities);
                });
            }

            tasksRoot.newSeparator(activitiesDesktopsMenu);

            for (let i = 0; i < runningActivities.length; ++i) {
                const activityId = runningActivities[i];

                // if the task is on a single activity, don't insert a "move to" item for that activity
                if (onActivities.length === 1 && onActivities[0] === activityId) {
                    continue;
                }

                menuItem = tasksRoot.newMenuItem(activitiesDesktopsMenu);
                menuItem.text = i18n("Move to %1", activityInfo.activityName(activityId));
                menuItem.icon = activityInfo.activityIcon(activityId);
                menuItem.clicked.connect(() => {
                    tasksModel.requestActivities(menu.modelIndex, [activityId]);
                });
            }
        }

        // Add minimize/maximize controls
        let menuItem = tasksRoot.newMenuItem(menu);
        menuItem.text = i18n("Mi&nimize All");
        menuItem.icon = "window-minimize";
        menuItem.enabled = Boolean(menu.get(TaskManager.AbstractTasksModel.IsMinimizable));
        menuItem.clicked.connect(() => {
            tasksModel.requestToggleMinimized(menu.modelIndex);
        });

        menuItem = tasksRoot.newMenuItem(menu);
        menuItem.text = i18n("Ma&ximize All");
        menuItem.icon = "window-maximize";
        menuItem.enabled = Boolean(menu.get(TaskManager.AbstractTasksModel.IsMaximizable));
        menuItem.clicked.connect(() => {
            tasksModel.requestToggleMaximized(menu.modelIndex);
        });

        tasksRoot.newSeparator(menu);

        // Add close
        menuItem = tasksRoot.newMenuItem(menu);
        menuItem.text = i18n("&Close All");
        menuItem.icon = "window-close";
        menuItem.enabled = Boolean(menu.get(TaskManager.AbstractTasksModel.IsClosable));
        menuItem.clicked.connect(() => {
            tasksModel.requestClose(menu.modelIndex);
        });

    }
}
