/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
pragma ComponentBehavior: Bound

import QtQuick
import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.extras as PlasmaExtras

PlasmaExtras.Menu {
    id: menu
    required property ContainmentItem panel
    property PlasmoidItem applet: null

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

    onStatusChanged: {
        if (status === PlasmaExtras.Menu.Closed) {
            menu.destroy();
        }
    }

    readonly property Component _menuComponent:         Component { PlasmaExtras.Menu {} }
    readonly property Component _menuItemComponent:     Component { PlasmaExtras.MenuItem {} }
    readonly property Component _separatorComponent:    Component { PlasmaExtras.MenuItem { separator: true } }
    function newMenu(parent: QtObject): PlasmaExtras.Menu             { return _menuComponent.createObject(parent) as PlasmaExtras.Menu}
    function newMenuItem(parent: QtObject): PlasmaExtras.MenuItem     { return _menuItemComponent.createObject(parent) as PlasmaExtras.MenuItem}
    function newSeparator(parent: QtObject): PlasmaExtras.MenuItem    { return _separatorComponent.createObject(parent) as PlasmaExtras.MenuItem}

    function show(x = undefined, y = undefined): void {
        populateMenu();
        if (x !== undefined && y !== undefined)
            // If x,y (position relative to visualParent) are set, then popup at that position
            open(x, y);
        else
            // Otherwise, just call openRelative
            openRelative();

        // destroy the menu if it fails to open for whatever reason
        if (status === PlasmaExtras.Menu.Closed)
            menu.destroy();
    }

    // Analogous to QMenu::addAction, just here the target menu is specified
    function addAction(target: PlasmaExtras.Menu, action: PlasmaCore.Action): PlasmaExtras.MenuItem {
        if (!action || !action.visible)
            return null;
        const item = newMenuItem(target);
        item.action = action;
        return item;
    }

    function populateMenu(): void {
        clearMenuItems();

        if (applet && !panel.Plasmoid.immutable) {
            // If we have an applet, we place all of the applets contextual/internal
            // actions in the menu root, and the panel menu goes in a submenu.
            // Note: if the panel is immutable, we skip showing the applet's actions!
            applet.prepareContextualActions();
            const contextualActions = panel.Plasmoid.allContextualActions(applet.Plasmoid);
            if (contextualActions.length > 0) {
                // tracks whether or not we've added any items
                let anyAdded = false;

                // First add any contextual actions for the applet
                for (let i = 0; i < contextualActions.length; ++i) {
                    if (addAction(menu, contextualActions[i]))
                        anyAdded = true;
                }

                // Separate the contextual actions from the internal actions
                if (anyAdded)
                    newSeparator(menu);
            }

            // Now append the internal actions!
            addAction(menu, applet.Plasmoid.internalAction("move"));
            addAction(menu, applet.Plasmoid.internalAction("remove"));

            const configure = applet.Plasmoid.internalAction("configure");
            if (configure && configure.enabled)
                // The configure action is a bit unique; we must hide it if it's disabled! otherwise it looks stupid
                addAction(menu, configure);

            addAction(menu, applet.Plasmoid.internalAction("alternatives"));

            newSeparator(menu);

            // Add the panel's menu
            const panelMenuItem = newMenuItem(menu);
            panelMenuItem.text = i18n("Panel Menu");

            const panelMenu = newMenu(panelMenuItem);
            populatePanelMenu(panelMenu);
            panelMenu.visualParent = panelMenuItem.action;
        } else
            // otherwise, directly populate the top level menu
            populatePanelMenu(menu);
    }

    // Populates a given menu with the panel's actions
    function populatePanelMenu(target: PlasmaExtras.Menu): void {
        panel.prepareContextualActions();

        // Contextual actions first...
        const contextualActions = panel.Plasmoid.allContextualActions(panel.Plasmoid);
        if (contextualActions.length > 0) {
            let anyAdded = false;

            for (let i = 0; i < contextualActions.length; ++i) {
                if (addAction(target, contextualActions[i]))
                    anyAdded = true;
            }

            // Separate the contextual actions from the internal actions
            if (anyAdded)
                newSeparator(target);
        }

        // Internal actions now
        if (!panel.Plasmoid.immutable) {
            // These actions wil always get added. There's technically the
            // possibility of having two separators if add/remove panel are
            // never added, but they always will be added, and QMenu will
            // swallow the extra separator anyways.
            addAction(target, panel.Plasmoid.internalAction("add widgets"));
            addAction(target, panel.Plasmoid.internalAction("add applications"));
            addAction(target, panel.Plasmoid.internalAction("remove applet"));

            newSeparator(target);

            addAction(target, panel.Plasmoid.internalAction("add panel"));
            addAction(target, panel.Plasmoid.internalAction("remove panel"));

            newSeparator(target);
        }

        addAction(target, panel.Plasmoid.internalAction("lock"));

        if (!panel.Plasmoid.immutable) {
            const configureMenuItem = newMenuItem(target);
            configureMenuItem.text = i18n("Configure Panel");
            configureMenuItem.icon = "configure";

            const configureMenu = newMenu(configureMenuItem);
            addAction(configureMenu, panel.Plasmoid.internalAction("configure"));
            addAction(configureMenu, panel.Plasmoid.internalAction("configure klassik"));
            configureMenu.visualParent = configureMenuItem.action;

            newSeparator(target);
        }

        addAction(target, panel.Plasmoid.internalAction("help"));
    }
}
