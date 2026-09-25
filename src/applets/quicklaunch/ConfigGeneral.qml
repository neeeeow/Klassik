/*
 *  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>
 *
 *  SPDX-FileCopyrightText: 2015 David Rosca <nowrep@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2

import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
import org.kde.kirigami as Kirigami
import org.kde.kcmutils as KCM

KCM.SimpleKCM {
    id: root

    readonly property bool vertical: Plasmoid.formFactor == PlasmaCore.Types.Vertical || (Plasmoid.formFactor == PlasmaCore.Types.Planar && Plasmoid.height > Plasmoid.width)

    property alias cfg_sectionCount: sectionCount.value

    Kirigami.FormLayout {
        QQC2.SpinBox {
            id: sectionCount

            Kirigami.FormData.label: root.vertical ? i18nc("@label:spinbox", "Number of columns:") : i18nc("@label:spinbox", "Number of rows:")

            from: 1
        }
    }
}
