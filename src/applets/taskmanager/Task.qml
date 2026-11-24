/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-FileCopyrightText: 2012-2013 Eike Hein <hein@kde.org>
    SPDX-FileCopyrightText: 2024 Nate Graham <nate@kde.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.extras as PlasmaExtras
import org.kde.kirigami as Kirigami
import org.kde.plasma.plasmoid

import org.kde.taskmanager as TaskManager

ToolButton {
    id: task

// BEGIN properties

    // To achieve a bottom-to-top layout on vertical panels, the task manager
    // is rotated by 180 degrees(see main.qml). This makes the tasks rotated,
    // so un-rotate them here to fix that.
    rotation: tasksRoot.rotation

    Layout.fillWidth: true
    Layout.fillHeight: true
    Layout.maximumWidth: tasksRoot.vertical ? -1 : LayoutMetrics.preferredMaxWidth()
    Layout.maximumHeight: tasksRoot.vertical ? LayoutMetrics.preferredMaxHeight() : -1
    Layout.preferredWidth: Layout.maximumWidth // Ensure the tasks always try to fill up the maximum space they have available
    Layout.preferredHeight: Layout.maximumHeight

    leftPadding: 4
    rightPadding: 4
    topPadding: 0
    bottomPadding: 0

    focusPolicy: Qt.NoFocus
    checked: task.model.IsActive
    hoverEnabled: true

    required property var model
    required property int index
    required property /*main.qml*/ Item tasksRoot

    property bool isWindow: model.IsWindow
    property int childCount: model.ChildCount
    property int previousChildCount: 0
    property PlasmaExtras.Menu contextMenu: null
    property GroupMenu groupMenu: null

    property bool completed: false

    enum MiddleClickAction {
        None = 0,
        Close,
        NewInstance,
        ToggleMinimized,
        ToggleGrouping,
        BringToCurrentDesktop
    }

// END properties

// BEGIN functions

    function modelIndex(): /*QModelIndex*/ var {
        return tasksRoot.tasksModel.makeModelIndex(index);
    }

    function showContextMenu(): void {
        toolTipArea.hideImmediately();
        const index = modelIndex();
        const isGroup = tasksRoot.tasksModel.data(index, TaskManager.AbstractTasksModel.IsGroupParent);
        const component = isGroup ? tasksRoot.groupContextMenuComponent : tasksRoot.contextMenuComponent;
        contextMenu = component.createObject(task, {
            tasksRoot: task.tasksRoot,
            visualParent: task,
            modelIndex: index
        }) as PlasmaExtras.Menu;
        contextMenu?.show();
    }

    function showGroupMenu(): void {
        toolTipArea.hideImmediately();
        groupMenu = tasksRoot.groupMenuComponent.createObject(task, {
            tasksRoot: task.tasksRoot,
            visualParent: task,
            modelIndex: modelIndex()
        }) as GroupMenu;
        groupMenu?.show();
    }

// END functions

// BEGIN connections

    onXChanged: if (completed) tasksRoot.requestLayout()
    onYChanged: if (completed) tasksRoot.requestLayout()

    onIsWindowChanged: {
        if (model.IsWindow) {
            tasksRoot.taskInitComponent.createObject(task);
        }
    }

    onChildCountChanged: {
        if (TaskTools.taskManagerInstanceCount < 2 && childCount > previousChildCount) {
            tasksRoot.tasksModel.requestPublishDelegateGeometry(modelIndex(), tasksRoot.globalRect(task), task);
        }

        previousChildCount = childCount;
    }

    onIndexChanged: {
        toolTipArea.hideToolTip();
        if (!tasksRoot.vertical) {
            tasksRoot.requestLayout();
        }
    }

    onClicked: {
        toolTipArea.hideToolTip();
        TaskTools.activateTask(modelIndex(), model, task, Plasmoid, tasksRoot);
    }

    Component.onCompleted: {
        if (model.IsWindow) {
            tasksRoot.taskInitComponent.createObject(task);
        }
        completed = true;
    }

// END connections

// BEGIN UI elements

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton | Qt.MiddleButton

        onClicked: (mouse) => {
            if (mouse.button === Qt.RightButton) {
                task.showContextMenu();
            } else if (mouse.button === Qt.MiddleButton) {
                switch (Plasmoid.configuration.middleClickAction) {
                    case Task.MiddleClickAction.Close:
                        task.tasksRoot.tasksModel.requestClose(task.modelIndex());
                        break;
                    case Task.MiddleClickAction.NewInstance:
                        task.tasksRoot.tasksModel.requestNewInstance(task.modelIndex());
                        break;
                    case Task.MiddleClickAction.ToggleMinimized:
                        task.tasksRoot.tasksModel.requestToggleMinimized(task.modelIndex());
                        break;
                    case Task.MiddleClickAction.ToggleGrouping:
                        task.tasksRoot.tasksModel.requestToggleGrouping(task.modelIndex());
                        break;
                    case Task.MiddleClickAction.BringToCurrentDesktop:
                        TaskTools.foreachChildTask((childIndex) => {
                            task.tasksRoot.tasksModel.requestVirtualDesktops(childIndex, [task.tasksRoot.virtualDesktopInfo.currentDesktopByScreenGeometry(task.tasksRoot.tasksModel.data(childIndex, TaskManager.AbstractTasksModel.ScreenGeometry))]);
                        }, task.modelIndex(), task.tasksRoot.tasksModel);
                        break;
                    default:
                        break;
                }
            }
        }
    }

    PlasmaCore.ToolTipArea {
        id: toolTipArea
        anchors.fill: parent

        active: task.groupMenu?.status !== PlasmaExtras.Menu.Open && task.contextMenu?.status !== PlasmaExtras.Menu.Open
        location: Plasmoid.location
        mainText: task.model.display
        icon: task.model.decoration
    }

    background: TaskBackground {
        visible: (Plasmoid.configuration.taskAppearance === 1) || task.hovered
        sunken: task.down || task.checked
        hovered: task.hovered
    }

    contentItem: RowLayout {
        spacing: Kirigami.Units.smallSpacing
        opacity: task.model.IsMinimized ? 0.5 : 1.0

        Item {
            id: iconBox
            Layout.preferredHeight: Kirigami.Units.iconSizes.small
            Layout.preferredWidth: Layout.preferredHeight

            Kirigami.Icon {
                anchors.fill: parent
                source: task.model.decoration
            }

            Loader {
                anchors.fill: parent
                active: task.model.IsStartup
                sourceComponent: task.tasksRoot.busyIndicator
            }
        }

        Label {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter

            visible: (task.width - iconBox.width - Kirigami.Units.smallSpacing) >= LayoutMetrics.spaceRequiredToShowText()

            text: task.model.display
            elide: Text.ElideRight
            textFormat: Text.PlainText
            maximumLineCount: 1
            font.bold: task.model.IsActive || task.model.IsDemandingAttention
        }
    }

// END UI elements
}
