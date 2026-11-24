/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.extras as PlasmaExtras
import org.kde.plasma.plasmoid

import org.kde.taskmanager as TaskManager

import com.github.neeeeow.klassik.painteditems

ToolButton {
    id: windowListButton
    required property /*main.qml*/ Item tasksRoot

    focusPolicy: Qt.NoFocus
    checked: windowListMenu.status === PlasmaExtras.Menu.Open
    onClicked: windowListMenu.show()
    enabled: allTasksModel.count > 0
    hoverEnabled: true
    rotation: windowListButton.tasksRoot.rotation

    // Task model used to display *all* tasks across each desktop
    readonly property TaskManager.TasksModel allTasksModel: TaskManager.TasksModel {
        filterByVirtualDesktop: false
        filterByScreen: false
        filterByActivity: false
        filterNotMinimized: false
        groupMode: TaskManager.TasksModel.GroupDisabled
        sortMode: TaskManager.TasksModel.SortVirtualDesktop
    }

    background: TaskBackground {
        // NB: we want this button to *always* be visible, regardless if tasks are classic or elegant
        sunken: windowListButton.down || windowListButton.checked
        hovered: windowListButton.hovered
    }

    StyledArrow {
        width: Math.min(parent.width, parent.height) - anchors.margins * 2
        height: width
        sunken: windowListButton.down || windowListButton.checked
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

    WindowListMenu {
        id: windowListMenu
        tasksRoot: windowListButton.tasksRoot
        visualParent: windowListButton
        tasksModel: windowListButton.allTasksModel
    }

    PlasmaCore.ToolTipArea {
        anchors.fill: parent
        active: windowListMenu.status !== PlasmaExtras.Menu.Open
        location: Plasmoid.location
        mainText: i18n("Window List")
        icon: Plasmoid.icon
    }
}
