/*
    SPDX-FileCopyrightText: 2013 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kcmutils as KCMUtils
import org.kde.kirigami as Kirigami
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid

import org.kde.plasma.workspace.dbus as DBus
import org.kde.taskmanager as TaskManager

KCMUtils.SimpleKCM {
    id: root

    property alias cfg_groupingStrategy: groupingStrategy.currentIndex
    property alias cfg_onlyGroupWhenFull: onlyGroupWhenFull.checked
    property int cfg_sortingStrategy
    property alias cfg_middleClickAction: middleClickAction.currentIndex
    property alias cfg_wheelEnabled: wheelEnabled.currentIndex
    property alias cfg_wheelSkipMinimized: wheelSkipMinimized.checked
    property alias cfg_showOnlyCurrentScreen: showOnlyCurrentScreen.checked
    property alias cfg_showOnlyCurrentDesktop: showOnlyCurrentDesktop.checked
    property alias cfg_showOnlyCurrentActivity: showOnlyCurrentActivity.checked
    property alias cfg_showOnlyMinimized: showOnlyMinimized.checked
    property alias cfg_minimizeActiveTaskOnClick: minimizeActive.checked
    property alias cfg_unhideOnAttention: unhideOnAttention.checked
    property alias cfg_reverseMode: reverseMode.checked

    headerPaddingEnabled: false
    header: ColumnLayout {
        spacing: Kirigami.Units.smallSpacing

        Kirigami.InlineMessage {
            id: annoyingAppWorkaroundMessage

            Layout.fillWidth: true
            position: Kirigami.InlineMessage.Position.Header

            text: i18nc("@info", "If you're using this setting to work around an application that demands attention too often, first look for a setting in the app to disable that behavior. If you don't find one, consider reporting this as a bug to the app's developer.")
        }
    }

    Kirigami.FormLayout {
        anchors.left: parent.left
        anchors.right: parent.right

        QQC2.ComboBox {
            id: groupingStrategy
            Kirigami.FormData.label: i18nc("@label:listbox how to group tasks", "Group:")
            Layout.fillWidth: true
            Layout.minimumWidth: Kirigami.Units.gridUnit * 14
            model: [
                i18nc("@item:inlistbox how to group tasks", "Do not group"),
                i18nc("@item:inlistbox how to group tasks", "By program name")
            ]
        }

        Item {
            Kirigami.FormData.isSection: true
        }

        QQC2.CheckBox {
            id: onlyGroupWhenFull
            visible: (Plasmoid.pluginName !== "org.kde.plasma.icontasks")
            text: i18nc("@option:check grouped task","Group only when the Task Manager is full")
            enabled: groupingStrategy.currentIndex > 0
            Accessible.onPressAction: toggle()
        }

        Item {
            Kirigami.FormData.isSection: true
            visible: (Plasmoid.pluginName !== "org.kde.plasma.icontasks")
        }

        QQC2.ComboBox {
            id: sortingStrategy
            Kirigami.FormData.label: i18nc("@label:listbox sort tasks in grouped task", "Sort:")
            Layout.fillWidth: true
            Layout.minimumWidth: Kirigami.Units.gridUnit * 14
            textRole: "text"
            valueRole: "value"
            model: [
                {
                    "text": i18nc("@item:inlistbox sort tasks in grouped task", "Do not sort"),
                    "value": TaskManager.TasksModel.SortDisabled,
                },
                {
                    "text": i18nc("@item:inlistbox sort tasks in grouped task", "Manually"),
                    "value": TaskManager.TasksModel.SortManual,
                },
                {
                    "text": i18nc("@item:inlistbox sort tasks in grouped task", "Alphabetically"),
                    "value": TaskManager.TasksModel.SortAlpha,
                },
                {
                    "text": i18nc("@item:inlistbox sort tasks in grouped task", "By desktop"),
                    "value": TaskManager.TasksModel.SortVirtualDesktop,
                },
                {
                    "text": i18nc("@item:inlistbox sort tasks in grouped task", "By activity"),
                    "value": TaskManager.TasksModel.SortActivity,
                },
                {
                    "text": i18nc("@item:inlistbox sort tasks in grouped task", "By horizontal window position"),
                    "value": TaskManager.TasksModel.SortWindowPositionHorizontal,
                },
            ]
            onActivated: root.cfg_sortingStrategy = currentValue
            Component.onCompleted: currentIndex = indexOfValue(root.cfg_sortingStrategy)
        }

        Item {
            Kirigami.FormData.isSection: true
            visible: (Plasmoid.pluginName !== "org.kde.plasma.icontasks")
        }

        QQC2.CheckBox {
            id: minimizeActive
            Kirigami.FormData.label: i18nc("@label for checkbox Part of a sentence: 'Clicking active task minimizes the task'", "Clicking active task:")
            text: i18nc("@option:check Part of a sentence: 'Clicking active task minimizes the task'", "Minimizes the task")
        }

        QQC2.ComboBox {
            id: middleClickAction
            Kirigami.FormData.label: i18nc("@label:listbox completes sentence like: … does nothing", "Middle-clicking any task:")
            Layout.fillWidth: true
            Layout.minimumWidth: Kirigami.Units.gridUnit * 14
            model: [
                i18nc("@item:inlistbox Part of a sentence: 'Middle-clicking any task does nothing'", "Does nothing"),
                i18nc("@item:inlistbox Part of a sentence: 'Middle-clicking any task closes window or group'", "Closes window or group"),
                i18nc("@item:inlistbox Part of a sentence: 'Middle-clicking any task opens a new window'", "Opens a new window"),
                i18nc("@item:inlistbox Part of a sentence: 'Middle-clicking any task minimizes/restores window or group'", "Minimizes/Restores window or group"),
                i18nc("@item:inlistbox Part of a sentence: 'Middle-clicking any task toggles grouping'", "Toggles grouping"),
                i18nc("@item:inlistbox Part of a sentence: 'Middle-clicking any task brings it to the current virtual desktop'", "Brings it to the current virtual desktop")
            ]
        }

        Item {
            Kirigami.FormData.isSection: true
        }

        QQC2.ComboBox {
            id: wheelEnabled
            Kirigami.FormData.label: i18n("Scrolling behavior")
            Layout.fillWidth: true
            Layout.minimumWidth: Kirigami.Units.gridUnit * 14
            model: [
                i18nc("@item:inlistbox Part of a sentence: 'Scrolling behavior does nothing'", "Does nothing"),
                i18nc("@item:inlistbox Part of a sentence: 'Scrolling behavior cycles through all tasks'", "Cycles through all tasks"),
                i18nc("@item:inlistbox Part of a sentence: 'Scrolling behavior cycles through the hovered task's windows'", "Cycles through the hovered task’s windows")
            ]
        }

        QQC2.CheckBox {
            id: wheelSkipMinimized
            leftPadding: mirrored ? 0 : (wheelEnabled.indicator.width + wheelEnabled.spacing)
            rightPadding: mirrored ? (wheelEnabled.indicator.width + wheelEnabled.spacing) : 0
            text: i18nc("@option:check mouse wheel task cycling", "Skip minimized tasks")
            enabled: wheelEnabled.currentIndex !== 0 // None
        }

        Item {
            Kirigami.FormData.isSection: true
        }

        QQC2.CheckBox {
            id: showOnlyCurrentDesktop
            Kirigami.FormData.label: i18nc("@label for checkbox group, completes sentence like: … from current screen", "Show only tasks:")
            text: i18nc("@option:check completes sentence: show only tasks", "From the current desktop")
        }

        QQC2.CheckBox {
            id: showOnlyCurrentActivity
            text: i18nc("@option:check completes sentence: show only tasks", "From the current activity")
        }

        QQC2.CheckBox {
            id: showOnlyCurrentScreen
            text: i18nc("@option:check completes sentence: show only tasks", "From the current screen")
        }

        QQC2.CheckBox {
            id: showOnlyMinimized
            text: i18nc("@option:check completes sentence: show only tasks", "That are minimized")
        }

        Item {
            Kirigami.FormData.isSection: true
        }

        QQC2.CheckBox {
            id: unhideOnAttention
            Kirigami.FormData.label: i18nc("@label for checkbox, completes sentence: … unhide if window wants attention", "When panel is hidden:")
            text: i18nc("@option:check completes sentence: When panel is hidden", "Unhide when a window wants attention")
            onToggled: {
                annoyingAppWorkaroundMessage.visible = !unhideOnAttention.checked;
            }
        }

        Item {
            Kirigami.FormData.isSection: true
        }

        QQC2.ButtonGroup {
            id: reverseModeRadioButtonGroup
        }

        QQC2.RadioButton {
            Kirigami.FormData.label: i18nc("@label for radiobutton group completes sentence like: … on the bottom", "New tasks appear:")
            checked: !reverseMode.checked
            text: {
                if (Plasmoid.formFactor === PlasmaCore.Types.Vertical) {
                    return i18nc("@option:check completes sentence: New tasks appear", "On the bottom")
                }
                // horizontal
                if (Application.layoutDirection === Qt.LeftToRight) {
                    return i18nc("@option:check completes sentence: New tasks appear", "To the right");
                } else {
                    return i18nc("@option:check completes sentence: New tasks appear", "To the left")
                }
            }
            QQC2.ButtonGroup.group: reverseModeRadioButtonGroup
        }

        QQC2.RadioButton {
            id: reverseMode
            checked: Plasmoid.configuration.reverseMode === true
            text: {
                if (Plasmoid.formFactor === PlasmaCore.Types.Vertical) {
                    return i18nc("@option:check completes sentence: New tasks appear", "On the top")
                }
                // horizontal
                if (Application.layoutDirection === Qt.LeftToRight) {
                    return i18nc("@option:check completes sentence: New tasks appear", "To the left");
                } else {
                    return i18nc("@option:check completes sentence: New tasks appear", "To the right");
                }
            }
            QQC2.ButtonGroup.group: reverseModeRadioButtonGroup
        }
    }
}
