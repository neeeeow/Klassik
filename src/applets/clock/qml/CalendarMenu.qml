/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
pragma ComponentBehavior: Bound

import QtQuick
import org.kde.plasma.extras as PlasmaExtras
import com.github.neeeeow.klassik.actionfactory

PlasmaExtras.Menu {
    id: menu

    readonly property Component _menuItemComponent: Component { PlasmaExtras.MenuItem {} }
    function newMenuItem(parent: QtObject): PlasmaExtras.MenuItem { return _menuItemComponent.createObject(parent) as PlasmaExtras.MenuItem}

    Component.onCompleted: {
        const title = newMenuItem(menu);
        // NB: KDE 3 used 'date' for the icon, but office-calendar is the icon used in KDE 6,
        // so we follow that convention for compatibility with newer icon sets.
        title.action = ActionFactory.popupTitle("office-calendar", i18n("Calendar"), title);

        const calendar = newMenuItem(menu);
        calendar.action = ActionFactory.popupCalendar(calendar);
    }
}
