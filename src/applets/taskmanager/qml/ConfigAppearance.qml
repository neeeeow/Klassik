/*
    SPDX-FileCopyrightText: 2013 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick
import QtQuick.Controls as QQC2

import org.kde.kcmutils as KCMUtils
import org.kde.kirigami as Kirigami
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid

KCMUtils.SimpleKCM {
    id: root

    readonly property bool plasmoidVertical: Plasmoid.formFactor === PlasmaCore.Types.Vertical

    property alias cfg_taskMaxWidth: taskMaxWidth.currentIndex
    property alias cfg_taskMaxHeight: taskMaxHeight.currentIndex

    property alias cfg_drawFrame: drawFrame.checked
    property alias cfg_taskAppearance: taskAppearance.currentIndex

    Kirigami.FormLayout {
        QQC2.CheckBox {
            id: drawFrame
            text: i18n("Draw a frame around the task manager")
        }

        Item {
            Kirigami.FormData.isSection: true
        }

        QQC2.ComboBox {
            id: taskAppearance

            Kirigami.FormData.label: i18n("Task appearance")

            model: [
                i18n("Elegant"),
                i18n("Classic")
            ]
        }

        QQC2.ComboBox {
            id: taskMaxWidth
            visible: !root.plasmoidVertical

            Kirigami.FormData.label: i18nc("@label:listbox", "Maximum task width:")

            model: [
                i18nc("@item:inlistbox how wide a task item should be", "Narrow"),
                i18nc("@item:inlistbox how wide a task item should be", "Medium"),
                i18nc("@item:inlistbox how wide a task item should be", "Wide")
            ]
        }

        QQC2.ComboBox {
            id: taskMaxHeight

            Kirigami.FormData.label: i18nc("@label:listbox", "Maximum task height:")

            model: [
                i18nc("@item:inlistbox how tall a task item should be", "Short"),
                i18nc("@item:inlistbox how tall a task item should be", "Medium"),
                i18nc("@item:inlistbox how tall a task item should be", "Tall")
            ]
        }
    }
}
