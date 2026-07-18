/*
 *  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick

import org.kde.plasma.configuration

ConfigModel {
    ConfigCategory {
        name: i18n("General")
        icon: "preferences-desktop-plasma"
        source: "ConfigGeneral.qml"
    }
}
