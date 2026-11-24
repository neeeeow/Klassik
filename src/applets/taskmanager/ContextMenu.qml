/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-FileCopyrightText: 2012-2016 Eike Hein <hein@kde.org>
    SPDX-FileCopyrightText: 2016 Kai Uwe Broulik <kde@privat.broulik.de>

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
    readonly property bool isSubMenu: visualParent !== null && !(visualParent as Item) // If the parent is a menu item, we must not destroy it on closure!

    placement: tasksRoot.menuPlacement

    minimumWidth: isSubMenu ? 0 : (visualParent as Item)?.width ?? 0

    onStatusChanged: {
        if (status === PlasmaExtras.Menu.Closed && !isSubMenu) {
            menu.destroy();
        }
    }

    function get(modelProp: int): var {
        return tasksRoot.tasksModel.data(modelIndex, modelProp)
    }

    function show(): void {
        openRelative();

        if (status === PlasmaExtras.Menu.Closed)
            menu.destroy();
    }

    PlasmaExtras.MenuItem {
        id: advancedMenuItem
        visible: menu.visualParent && !menu.get(TaskManager.AbstractTasksModel.IsStartup)
        enabled: visible

        text: i18n("Ad&vanced")

        readonly property PlasmaExtras.Menu advancedMenu: PlasmaExtras.Menu {
            visualParent: advancedMenuItem.action
            PlasmaExtras.MenuItem {
                checkable: true
                checked: menu.visualParent && menu.get(TaskManager.AbstractTasksModel.IsKeepAbove)

                text: i18n("Keep &Above Others")
                icon: "window-keep-above"

                onClicked: menu.tasksRoot.tasksModel.requestToggleKeepAbove(menu.modelIndex)
            }

            PlasmaExtras.MenuItem {
                checkable: true
                checked: menu.visualParent && menu.get(TaskManager.AbstractTasksModel.IsKeepBelow)

                text: i18n("Keep &Below Others")
                icon: "window-keep-below"

                onClicked: menu.tasksRoot.tasksModel.requestToggleKeepBelow(menu.modelIndex)
            }

            PlasmaExtras.MenuItem {
                enabled: menu.visualParent && menu.get(TaskManager.AbstractTasksModel.IsFullScreenable)

                checkable: true
                checked: menu.visualParent && menu.get(TaskManager.AbstractTasksModel.IsFullScreen)

                text: i18n("&Fullscreen")
                icon: "view-fullscreen"

                onClicked: menu.tasksRoot.tasksModel.requestToggleFullScreen(menu.modelIndex)
            }

            PlasmaExtras.MenuItem {
                enabled: menu.visualParent && menu.get(TaskManager.AbstractTasksModel.CanSetNoBorder)

                checkable: true
                checked: menu.visualParent && menu.get(TaskManager.AbstractTasksModel.HasNoBorder)

                text: i18n("&No Titlebar and Frame")
                icon: "edit-none-border"

                onClicked: menu.tasksRoot.tasksModel.requestToggleNoBorder(menu.modelIndex)
            }

            PlasmaExtras.MenuItem {
                enabled: menu.visualParent

                checkable: true
                checked: menu.visualParent && menu.get(TaskManager.AbstractTasksModel.IsExcludedFromCapture)
                visible: Qt.platform.pluginName === "wayland"

                text: i18n("&Hide from Screencast")
                icon: "view-private"

                onClicked: menu.tasksRoot.tasksModel.requestToggleExcludeFromCapture(menu.modelIndex)
            }

            PlasmaExtras.MenuItem {
                separator: true
            }

            PlasmaExtras.MenuItem {
                visible: (Plasmoid.configuration.groupingStrategy !== 0) && menu.get(TaskManager.AbstractTasksModel.IsWindow)

                checkable: true
                checked: menu.visualParent && menu.get(TaskManager.AbstractTasksModel.IsGroupable)

                text: i18n("Allow this program to be grouped")
                icon: "view-group"

                onClicked: menu.tasksRoot.tasksModel.requestToggleGrouping(menu.modelIndex)
            }
        }
    }

    PlasmaExtras.MenuItem {
        id: virtualDesktopsMenuItem

        visible: menu.tasksRoot.virtualDesktopInfo.numberOfDesktops > 1
            && (menu.visualParent
            && !menu.get(TaskManager.AbstractTasksModel.IsStartup)
            && menu.get(TaskManager.AbstractTasksModel.IsVirtualDesktopsChangeable))
        enabled: visible

        text: i18n("To &Desktop")
        icon: "virtual-desktops"

        readonly property PlasmaExtras.Menu _virtualDesktopsMenu: PlasmaExtras.Menu {
            id: virtualDesktopsMenu
            visualParent: virtualDesktopsMenuItem.action

            Component.onCompleted: {
                const virtualDesktopInfo = menu.tasksRoot.virtualDesktopInfo;
                const tasksModel = menu.tasksRoot.tasksModel;

                if (virtualDesktopInfo.numberOfDesktops <= 1 || !virtualDesktopsMenuItem.enabled) {
                    return;
                }

                const taskDesktops = menu.get(TaskManager.AbstractTasksModel.VirtualDesktops) ?? [];

                let menuItem = menu.tasksRoot.newMenuItem(virtualDesktopsMenu);
                menuItem.text = i18n("&To Current Desktop");
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

                menuItem = menu.tasksRoot.newMenuItem(virtualDesktopsMenu);
                menuItem.text = i18n("&All Desktops");
                menuItem.checkable = true;
                menuItem.checked = Boolean(menu.visualParent && menu.get(TaskManager.AbstractTasksModel.IsOnAllVirtualDesktops));
                menuItem.clicked.connect(() => {
                    tasksModel.requestVirtualDesktops(menu.modelIndex, []);
                });

                menu.tasksRoot.newSeparator(virtualDesktopsMenu);

                for (let i = 0; i < virtualDesktopInfo.desktopNames.length; ++i) {
                    const desktopId = virtualDesktopInfo.desktopIds[i];

                    menuItem = menu.tasksRoot.newMenuItem(virtualDesktopsMenu);
                    menuItem.text = virtualDesktopInfo.desktopNames[i];
                    menuItem.checkable = true;
                    menuItem.checked = Boolean(menu.visualParent && taskDesktops.indexOf(desktopId) > -1);
                    menuItem.clicked.connect(() => {
                        tasksModel.requestVirtualDesktops(menu.modelIndex, [desktopId]);
                    });
                }
            }
        }
    }

    PlasmaExtras.MenuItem {
        id: activitiesDesktopsMenuItem

        visible: menu.tasksRoot.activityInfo.numberOfRunningActivities > 1
        && (menu.visualParent && !menu.get(TaskManager.AbstractTasksModel.IsStartup))

        enabled: visible

        text: i18n("Show in &Activities")
        icon: "activities"

        readonly property PlasmaExtras.Menu _activitiesDesktopsMenu: PlasmaExtras.Menu {
            id: activitiesDesktopsMenu

            visualParent: activitiesDesktopsMenuItem.action

            Component.onCompleted: {
                const activityInfo = menu.tasksRoot.activityInfo;
                const tasksModel = menu.tasksRoot.tasksModel;

                if (activityInfo.numberOfRunningActivities <= 1) {
                    return;
                }

                const onActivities = menu.get(TaskManager.AbstractTasksModel.Activities) ?? [];

                let menuItem = menu.tasksRoot.newMenuItem(activitiesDesktopsMenu);
                menuItem.text = i18n("Add to Current Activity");
                menuItem.enabled = Boolean(menu.visualParent && onActivities.length > 0 &&
                    onActivities.indexOf(activityInfo.currentActivity) < 0);
                menuItem.clicked.connect(() => {
                    tasksModel.requestActivities(menu.modelIndex, onActivities.concat(activityInfo.currentActivity));
                });

                menuItem = menu.tasksRoot.newMenuItem(activitiesDesktopsMenu);
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

                menu.tasksRoot.newSeparator(activitiesDesktopsMenu);

                const runningActivities = activityInfo.runningActivities();
                for (let i = 0; i < runningActivities.length; ++i) {
                    const activityId = runningActivities[i];

                    menuItem = menu.tasksRoot.newMenuItem(activitiesDesktopsMenu);
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

                menu.tasksRoot.newSeparator(activitiesDesktopsMenu);

                for (let i = 0; i < runningActivities.length; ++i) {
                    const activityId = runningActivities[i];

                    // if the task is on a single activity, don't insert a "move to" item for that activity
                    if (onActivities.length === 1 && onActivities[0] === activityId) {
                        continue;
                    }

                    menuItem = menu.tasksRoot.newMenuItem(activitiesDesktopsMenu);
                    menuItem.text = i18n("Move to %1", activityInfo.activityName(activityId));
                    menuItem.icon = activityInfo.activityIcon(activityId);
                    menuItem.clicked.connect(() => {
                        tasksModel.requestActivities(menu.modelIndex, [activityId]);
                    });
                }
            }
        }
    }


    PlasmaExtras.MenuItem {
        visible: menu.visualParent && !menu.get(TaskManager.AbstractTasksModel.IsStartup)
        enabled: visible && menu.get(TaskManager.AbstractTasksModel.IsMovable)

        text: i18n("&Move")
        icon: "transform-move"

        onClicked: menu.tasksRoot.tasksModel.requestMove(menu.modelIndex)
    }

    PlasmaExtras.MenuItem {
        visible: menu.visualParent && !menu.get(TaskManager.AbstractTasksModel.IsStartup)
        enabled: visible && menu.get(TaskManager.AbstractTasksModel.IsResizable)

        text: i18n("Re&size")
        icon: "transform-scale"

        onClicked: menu.tasksRoot.tasksModel.requestResize(menu.modelIndex)
    }

    PlasmaExtras.MenuItem {
        visible: menu.visualParent && !menu.get(TaskManager.AbstractTasksModel.IsStartup)
        enabled: visible && menu.get(TaskManager.AbstractTasksModel.IsMinimizable)

        checkable: true
        checked: menu.visualParent && menu.get(TaskManager.AbstractTasksModel.IsMinimized)

        text: i18n("Mi&nimize")
        icon: "window-minimize"

        onClicked: menu.tasksRoot.tasksModel.requestToggleMinimized(menu.modelIndex)
    }

    PlasmaExtras.MenuItem {
        visible: menu.visualParent && !menu.get(TaskManager.AbstractTasksModel.IsStartup)
        enabled: visible && menu.get(TaskManager.AbstractTasksModel.IsMaximizable)

        checkable: true
        checked: menu.visualParent && menu.get(TaskManager.AbstractTasksModel.IsMaximized)

        text: i18n("Ma&ximize")
        icon: "window-maximize"

        onClicked: menu.tasksRoot.tasksModel.requestToggleMaximized(menu.modelIndex)
    }

    PlasmaExtras.MenuItem { separator: true }

    PlasmaExtras.MenuItem {
        visible: menu.visualParent && !menu.get(TaskManager.AbstractTasksModel.IsStartup)
        enabled: visible && menu.get(TaskManager.AbstractTasksModel.IsClosable)

        text: i18n("&Close")
        icon: "window-close"

        onClicked: menu.tasksRoot.tasksModel.requestClose(menu.modelIndex)
    }
}
