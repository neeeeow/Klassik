/*
      SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

      SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick

import org.kde.plasma.plasmoid

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.extras as PlasmaExtras

import org.kde.taskmanager as TaskManager
import plasma.applet.com.github.neeeeow.klassik.taskmanager as TaskManagerApplet

PlasmaExtras.Menu {
    id: menu

    required property TaskManagerApplet.Backend backend
    required property /*QModelIndex*/var modelIndex
    readonly property Component taskMenuComponent: Qt.createComponent("ContextMenu.qml") // Needed for per-task submenu

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
            menu.destroy();
        }
    }

    function get(modelProp: int): var {
        return tasksModel.data(modelIndex, modelProp)
    }

    function show(): void {
        Plasmoid.contextualActionsAboutToShow();
        populateMenu();
        openRelative();
    }

    function newMenuItem(parent: QtObject): PlasmaExtras.MenuItem {
        return Qt.createQmlObject(`
        import org.kde.plasma.extras as PlasmaExtras

        PlasmaExtras.MenuItem {}
        `, parent) as PlasmaExtras.MenuItem;
    }

    function newSeparator(parent: QtObject): PlasmaExtras.MenuItem {
        return Qt.createQmlObject(`
        import org.kde.plasma.extras as PlasmaExtras

        PlasmaExtras.MenuItem { separator: true }
        `, parent) as PlasmaExtras.MenuItem;
    }

    function newMenu(parent: QtObject): PlasmaExtras.Menu {
        return Qt.createQmlObject(`
        import org.kde.plasma.extras as PlasmaExtras

        PlasmaExtras.Menu {}
        `, parent) as PlasmaExtras.Menu;
    }

    function populateMenu(): void {
        clearMenuItems();

        // Add per-task controls
        TaskManagerApplet.TaskTools.foreachChildTask((childIndex) => {
            const item = newMenuItem(menu);

            item.text = tasksModel.data(childIndex, Qt.DisplayRole) || "";
            item.icon = tasksModel.data(childIndex, Qt.DecorationRole) || "";

            addMenuItem(item);

            taskMenuComponent.createObject(menu, {
                visualParent: item.action,
                modelIndex: childIndex,
                backend: menu.backend
            });
        }, menu.modelIndex, tasksModel);

        menu.newSeparator(menu)

        // Add desktops submenu
        if (virtualDesktopInfo.numberOfDesktops > 1 && menu.get(TaskManager.AbstractTasksModel.IsVirtualDesktopsChangeable)) {
            const virtualDesktopsMenuItem = menu.newMenuItem(menu);
            virtualDesktopsMenuItem.text = i18n("All to &Desktop");
            virtualDesktopsMenuItem.icon = "virtual-desktops";

            const virtualDesktopsMenu = menu.newMenu(menu);
            virtualDesktopsMenu.visualParent = virtualDesktopsMenuItem.action;

            let menuItem = menu.newMenuItem(virtualDesktopsMenu);
            menuItem.text = i18n("All &to Current Desktop");
            menuItem.enabled = Qt.binding(() => {
                if (!menu.visualParent) {
                    return false;
                }

                let isAnyTaskNotOnCurrentDesktop = false;
                TaskManagerApplet.TaskTools.foreachChildTask((childIndex) => {
                    const screenGeometry = tasksModel.data(childIndex, TaskManager.AbstractTasksModel.ScreenGeometry);
                    const currentDesktop = virtualDesktopInfo.currentDesktopByScreenGeometry(screenGeometry);
                    isAnyTaskNotOnCurrentDesktop = isAnyTaskNotOnCurrentDesktop || tasksModel.data(childIndex, TaskManager.AbstractTasksModel.VirtualDesktops).indexOf(currentDesktop) === -1;
                }, menu.modelIndex, tasksModel);

                return isAnyTaskNotOnCurrentDesktop;
            });
            menuItem.clicked.connect(() => {
                TaskManagerApplet.TaskTools.foreachChildTask((childIndex) => {
                    tasksModel.requestVirtualDesktops(childIndex, [virtualDesktopInfo.currentDesktopByScreenGeometry(tasksModel.data(childIndex, TaskManager.AbstractTasksModel.ScreenGeometry))]);
                }, menu.modelIndex, tasksModel);
            });

            menuItem = menu.newMenuItem(virtualDesktopsMenu);
            menuItem.text = i18n("&All Desktops");
            menuItem.checkable = true;
            menuItem.checked = Qt.binding(() => {
                return menu.visualParent && menu.get(TaskManager.AbstractTasksModel.IsOnAllVirtualDesktops);
            });
            menuItem.clicked.connect(() => {
                tasksModel.requestVirtualDesktops(menu.modelIndex, []);
            });
            menu.backend.setActionGroup(menuItem.action);

            menu.newSeparator(virtualDesktopsMenu);

            for (let i = 0; i < virtualDesktopInfo.desktopNames.length; ++i) {
                menuItem = menu.newMenuItem(virtualDesktopsMenu);
                menuItem.text = virtualDesktopInfo.desktopNames[i];
                menuItem.checkable = true;
                menuItem.checked = Qt.binding((i => {
                    return () => menu.visualParent && menu.get(TaskManager.AbstractTasksModel.VirtualDesktops).indexOf(virtualDesktopInfo.desktopIds[i]) > -1;
                })(i));
                menuItem.clicked.connect((i => {
                    return () => tasksModel.requestVirtualDesktops(menu.modelIndex, [virtualDesktopInfo.desktopIds[i]]);
                })(i));
                menu.backend.setActionGroup(menuItem.action);
            }
        }

        // Add activities sub menu
        if (activityInfo.numberOfRunningActivities > 1) {
            const activitiesDesktopsMenuItem = menu.newMenuItem(menu);
            activitiesDesktopsMenuItem.text = i18n("Show All in &Activities");
            activitiesDesktopsMenuItem.icon = "activities";

            const activitiesDesktopsMenu = menu.newMenu(menu);
            activitiesDesktopsMenu.visualParent = activitiesDesktopsMenuItem.action;

            let menuItem = menu.newMenuItem(activitiesDesktopsMenu);
            menuItem.text = i18n("Add All to Current Activity");
            menuItem.enabled = Qt.binding(() => {
                return menu.visualParent && menu.get(TaskManager.AbstractTasksModel.Activities).length > 0 &&
                menu.get(TaskManager.AbstractTasksModel.Activities).indexOf(activityInfo.currentActivity) < 0;
            });
            menuItem.clicked.connect(() => {
                tasksModel.requestActivities(menu.modelIndex, menu.get(TaskManager.AbstractTasksModel.Activities).concat(activityInfo.currentActivity));
            });

            menuItem = menu.newMenuItem(activitiesDesktopsMenu);
            menuItem.text = i18n("All Activities");
            menuItem.checkable = true;
            menuItem.checked = Qt.binding(() => {
                return menu.visualParent && menu.get(TaskManager.AbstractTasksModel.Activities).length === 0;
            });
            menuItem.toggled.connect(checked => {
                let newActivities = []; // will cast to an empty QStringList i.e all activities
                if (!checked) {
                    newActivities = [activityInfo.currentActivity];
                }
                tasksModel.requestActivities(menu.modelIndex, newActivities);
            });

            menu.newSeparator(activitiesDesktopsMenu);

            const runningActivities = activityInfo.runningActivities();
            for (let i = 0; i < runningActivities.length; ++i) {
                const activityId = runningActivities[i];

                menuItem = menu.newMenuItem(activitiesDesktopsMenu);
                menuItem.text = activityInfo.activityName(runningActivities[i]);
                menuItem.icon = activityInfo.activityIcon(runningActivities[i]);
                menuItem.checkable = true;
                menuItem.checked = Qt.binding((activityId => {
                    return () => menu.visualParent && menu.get(TaskManager.AbstractTasksModel.Activities).indexOf(activityId) >= 0;
                })(activityId));
                menuItem.toggled.connect((activityId => {
                    return checked => {
                        let newActivities = menu.get(TaskManager.AbstractTasksModel.Activities);
                        if (checked) {
                            newActivities = newActivities.concat(activityId);
                        } else {
                            const index = newActivities.indexOf(activityId);
                            if (index < 0) {
                                return;
                            }

                            newActivities.splice(index, 1);
                        }
                        return tasksModel.requestActivities(menu.modelIndex, newActivities);
                    };
                })(activityId));
            }

            menu.newSeparator(activitiesDesktopsMenu);

            for (let i = 0; i < runningActivities.length; ++i) {
                const activityId = runningActivities[i];
                const onActivities = menu.get(TaskManager.AbstractTasksModel.Activities);

                // if the task is on a single activity, don't insert a "move to" item for that activity
                if (onActivities.length === 1 && onActivities[0] === activityId) {
                    continue;
                }

                menuItem = menu.newMenuItem(activitiesDesktopsMenu);
                menuItem.text = i18n("Move to %1", activityInfo.activityName(activityId))
                menuItem.icon = activityInfo.activityIcon(activityId)
                menuItem.clicked.connect((activityId => {
                    return () => tasksModel.requestActivities(menu.modelIndex, [activityId]);
                })(activityId));
            }

            menu.newSeparator(activitiesDesktopsMenu);
        }

        // Add minimize/maximize controls
        let menuItem = menu.newMenuItem(menu);
        menuItem.text = i18n("Mi&nimize All");
        menuItem.icon = "window-minimize";
        menuItem.enabled = Qt.binding(() => {
            return menu.get(TaskManager.AbstractTasksModel.IsMinimizable);
        });
        menuItem.clicked.connect(() => {
            tasksModel.requestToggleMinimized(menu.modelIndex);
        });

        menuItem = menu.newMenuItem(menu);
        menuItem.text = i18n("Ma&ximize All");
        menuItem.icon = "window-maximize";
        menuItem.enabled = Qt.binding(() => {
            return menu.get(TaskManager.AbstractTasksModel.IsMaximizable);
        });
        menuItem.clicked.connect(() => {
            tasksModel.requestToggleMaximized(menu.modelIndex);
        });

        menu.newSeparator(menu)

        // Add close
        menuItem = menu.newMenuItem(menu);
        menuItem.text = i18n("&Close All");
        menuItem.icon = "window-close";
        menuItem.enabled = Qt.binding(() => {
            return menu.get(TaskManager.AbstractTasksModel.IsClosable);
        });
        menuItem.clicked.connect(() => {
            tasksModel.requestClose(menu.modelIndex);
        });

    }
}
