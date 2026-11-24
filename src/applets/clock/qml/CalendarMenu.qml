/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
pragma ComponentBehavior: Bound

import QtQuick

import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.extras as PlasmaExtras

import com.github.neeeeow.klassik.actionfactory

PlasmaExtras.Menu {
    id: menu

    readonly property Component _menuItemComponent: Component { PlasmaExtras.MenuItem {} }
    function newMenuItem(parent: QtObject): PlasmaExtras.MenuItem { return _menuItemComponent.createObject(parent) as PlasmaExtras.MenuItem}

    Component.onCompleted: {
        const title = newMenuItem(menu);
        title.action = ActionFactory.popupTitle("date", i18n("Calendar"), title);

        const calendar = newMenuItem(menu);
        calendar.action = ActionFactory.popupCalendar(calendar);
    }
}
