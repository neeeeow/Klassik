/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>
*/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.extras as PlasmaExtras
import org.kde.plasma.plasmoid

import org.kde.taskmanager as TaskManager

ToolButton {
    id: windowListButton
    focusPolicy: Qt.NoFocus
    checked: windowListButton.menu?.status === PlasmaExtras.Menu.Open
    onClicked: showMenu()
    enabled: allTasksModel.count > 0
    hoverEnabled: true

    rotation: Plasmoid.configuration.reverseMode && Plasmoid.formFactor === PlasmaCore.Types.Vertical ? 180 : 0

    background: TaskBackground {
        visible: (Plasmoid.configuration.taskAppearance === 1) || windowListButton.hovered || sunken
        sunken: windowListButton.down || windowListButton.checked
    }

    StyledArrow {
        width: Math.min(parent.width, parent.height) - anchors.margins * 2
        height: width
        sunken: windowListButton.down || windowListButton.checked
        hoverEnabled: false
        hovered: windowListButton.hovered

        anchors.margins: 1
        anchors.top: (Plasmoid.location === PlasmaCore.Types.BottomEdge ||
                    Plasmoid.location === PlasmaCore.Types.Floating ||
                    Plasmoid.location === PlasmaCore.Types.Desktop ||
                    Plasmoid.location === PlasmaCore.Types.FullScreen) ? parent.top : undefined
        anchors.bottom: Plasmoid.location === PlasmaCore.Types.TopEdge ? parent.bottom : undefined
        anchors.left: Plasmoid.location === PlasmaCore.Types.RightEdge ? parent.left : undefined
        anchors.right: Plasmoid.location === PlasmaCore.Types.LeftEdge ? parent.right : undefined
        anchors.horizontalCenter: (Plasmoid.location === PlasmaCore.Types.TopEdge ||
                    Plasmoid.location === PlasmaCore.Types.BottomEdge ||
                    Plasmoid.location === PlasmaCore.Types.Floating ||
                    Plasmoid.location === PlasmaCore.Types.Desktop ||
                    Plasmoid.location === PlasmaCore.Types.FullScreen) ? parent.horizontalCenter : undefined
        anchors.verticalCenter: (Plasmoid.location === PlasmaCore.Types.LeftEdge ||
                    Plasmoid.location === PlasmaCore.Types.RightEdge) ? parent.verticalCenter : undefined

        direction: {
            switch (Plasmoid.location) {
                case PlasmaCore.Types.TopEdge:
                    return StyledArrow.Down;
                case PlasmaCore.Types.LeftEdge:
                    return StyledArrow.Right;
                case PlasmaCore.Types.RightEdge:
                    return StyledArrow.Left;
                default:
                    return StyledArrow.Up;
            }
        }
    }

    PlasmaCore.ToolTipArea {
        anchors.fill: parent
        active: windowListButton.menu?.status !== PlasmaExtras.Menu.Open
        location: Plasmoid.location
        mainText: i18n("Window List")
        icon: Plasmoid.icon
    }

    // Task model used to display *all* tasks across each desktop
    readonly property TaskManager.TasksModel allTasksModel: TaskManager.TasksModel {
        filterByVirtualDesktop: false
        filterByScreen: false
        filterByActivity: false
        filterNotMinimized: false
        groupMode: TaskManager.TasksModel.GroupDisabled
        sortMode: TaskManager.TasksModel.SortVirtualDesktop
    }

    // Set up the menu which displays the task list
    readonly property Component menuComponent: Qt.createComponent("WindowListMenu.qml")
    property QtObject menu: null
    function showMenu(): void {
        menu = menuComponent.createObject(windowListButton, {
            visualParent: windowListButton,
            tasksModel: windowListButton.allTasksModel,
        }) as WindowListMenu;
        menu.show();
    }
}
