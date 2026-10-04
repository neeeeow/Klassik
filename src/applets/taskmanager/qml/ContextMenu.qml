/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-FileCopyrightText: 2012-2016 Eike Hein <hein@kde.org>
    SPDX-FileCopyrightText: 2016 Kai Uwe Broulik <kde@privat.broulik.de>

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
        if (status === PlasmaExtras.Menu.Open) {
            activitiesDesktopsMenu.refresh();
        } else if (status === PlasmaExtras.Menu.Closed) {
            menu.destroy();
        }
    }

    function get(modelProp: int): var {
        return tasksModel.data(modelIndex, modelProp)
    }

    function show(): void {
        Plasmoid.contextualActionsAboutToShow();

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

                onClicked: tasksModel.requestToggleKeepAbove(menu.modelIndex)
            }

            PlasmaExtras.MenuItem {
                checkable: true
                checked: menu.visualParent && menu.get(TaskManager.AbstractTasksModel.IsKeepBelow)

                text: i18n("Keep &Below Others")
                icon: "window-keep-below"

                onClicked: tasksModel.requestToggleKeepBelow(menu.modelIndex)
            }

            PlasmaExtras.MenuItem {
                enabled: menu.visualParent && menu.get(TaskManager.AbstractTasksModel.IsFullScreenable)

                checkable: true
                checked: menu.visualParent && menu.get(TaskManager.AbstractTasksModel.IsFullScreen)

                text: i18n("&Fullscreen")
                icon: "view-fullscreen"

                onClicked: tasksModel.requestToggleFullScreen(menu.modelIndex)
            }

            PlasmaExtras.MenuItem {
                enabled: menu.visualParent && menu.get(TaskManager.AbstractTasksModel.CanSetNoBoder)

                checkable: true
                checked: menu.visualParent && menu.get(TaskManager.AbstractTasksModel.HasNoBorder)

                text: i18n("&No Titlebar and Frame")
                icon: "edit-none-border"

                onClicked: tasksModel.requestToggleNoBorder(menu.modelIndex)
            }

            PlasmaExtras.MenuItem {
                enabled: menu.visualParent

                checkable: true
                checked: menu.visualParent && menu.get(TaskManager.AbstractTasksModel.IsExcludedFromCapture)
                visible: Qt.platform.pluginName === "wayland"

                text: i18n("&Hide from Screencast")
                icon: "view-private"

                onClicked: tasksModel.requestToggleExcludeFromCapture(menu.modelIndex)
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

                onClicked: tasksModel.requestToggleGrouping(menu.modelIndex)
            }
        }
    }

    PlasmaExtras.MenuItem {
        id: virtualDesktopsMenuItem

        visible: virtualDesktopInfo.numberOfDesktops > 1
            && (menu.visualParent
            && !menu.get(TaskManager.AbstractTasksModel.IsStartup)
            && menu.get(TaskManager.AbstractTasksModel.IsVirtualDesktopsChangeable))
        enabled: visible

        text: i18n("To &Desktop")
        icon: "virtual-desktops"

        readonly property Connections virtualDesktopsMenuConnections: Connections {
            target: virtualDesktopInfo

            function onNumberOfDesktopsChanged(): void {
                Qt.callLater(virtualDesktopsMenu.refresh);
            }
            function onDesktopIdsChanged(): void {
                Qt.callLater(virtualDesktopsMenu.refresh);
            }
            function onDesktopNamesChanged(): void {
                Qt.callLater(virtualDesktopsMenu.refresh);
            }
        }


        readonly property PlasmaExtras.Menu _virtualDesktopsMenu: PlasmaExtras.Menu {
            id: virtualDesktopsMenu
            visualParent: virtualDesktopsMenuItem.action

            function refresh(): void {
                clearMenuItems();

                if (virtualDesktopInfo.numberOfDesktops <= 1 || !virtualDesktopsMenuItem.enabled) {
                    return;
                }

                let menuItem = menu.newMenuItem(virtualDesktopsMenu);
                menuItem.text = i18n("Move &To Current Desktop");
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
            Component.onCompleted: refresh()
        }
    }

    PlasmaExtras.MenuItem {
        id: activitiesDesktopsMenuItem

        visible: activityInfo.numberOfRunningActivities > 1
        && (menu.visualParent && !menu.get(TaskManager.AbstractTasksModel.IsStartup))

        enabled: visible

        text: i18n("Show in &Activities")
        icon: "activities"

        readonly property Connections activityInfoConnections: Connections {
            target: activityInfo

            function onNumberOfRunningActivitiesChanged(): void {
                activitiesDesktopsMenu.refresh()
            }
        }

        readonly property PlasmaExtras.Menu _activitiesDesktopsMenu: PlasmaExtras.Menu {
            id: activitiesDesktopsMenu

            visualParent: activitiesDesktopsMenuItem.action

            function refresh(): void {
                clearMenuItems();

                if (activityInfo.numberOfRunningActivities <= 1) {
                    return;
                }

                let menuItem = menu.newMenuItem(activitiesDesktopsMenu);
                menuItem.text = i18n("Add To Current Activity");
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

            Component.onCompleted: refresh()
        }
    }


    PlasmaExtras.MenuItem {
        visible: menu.visualParent && !menu.get(TaskManager.AbstractTasksModel.IsStartup)
        enabled: visible && menu.get(TaskManager.AbstractTasksModel.IsMovable)

        text: i18n("&Move")
        icon: "transform-move"

        onClicked: tasksModel.requestMove(menu.modelIndex)
    }

    PlasmaExtras.MenuItem {
        visible: menu.visualParent && !menu.get(TaskManager.AbstractTasksModel.IsStartup)
        enabled: visible && menu.get(TaskManager.AbstractTasksModel.IsResizable)

        text: i18n("Re&size")
        icon: "transform-scale"

        onClicked: tasksModel.requestResize(menu.modelIndex)
    }


    PlasmaExtras.MenuItem {
        visible: menu.visualParent && !menu.get(TaskManager.AbstractTasksModel.IsStartup)
        enabled: visible && menu.get(TaskManager.AbstractTasksModel.IsMaximizable)

        checkable: true
        checked: menu.visualParent && menu.get(TaskManager.AbstractTasksModel.IsMaximized)

        text: i18n("Ma&ximize")
        icon: "window-maximize"

        onClicked: tasksModel.requestToggleMaximized(menu.modelIndex)
    }

    PlasmaExtras.MenuItem {
        visible: menu.visualParent && !menu.get(TaskManager.AbstractTasksModel.IsStartup)
        enabled: visible && menu.get(TaskManager.AbstractTasksModel.IsMinimizable)

        checkable: true
        checked: menu.visualParent && menu.get(TaskManager.AbstractTasksModel.IsMinimized)

        text: i18n("Mi&nimize")
        icon: "window-minimize"

        onClicked: tasksModel.requestToggleMinimized(menu.modelIndex)
    }

    PlasmaExtras.MenuItem { separator: true }

    PlasmaExtras.MenuItem {
        visible: menu.visualParent && !menu.get(TaskManager.AbstractTasksModel.IsStartup)
        enabled: visible && menu.get(TaskManager.AbstractTasksModel.IsClosable)

        text: menu.get(TaskManager.AbstractTasksModel.IsGroupParent) ? i18n("&Close All") : i18n("&Close")
        icon: "window-close"

        onClicked: tasksModel.requestClose(menu.modelIndex)
    }
}
