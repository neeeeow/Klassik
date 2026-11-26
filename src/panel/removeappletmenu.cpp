/*
  SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "removeappletmenu.h"

#include <KLocalizedString>
#include <KNotification>

#include <Plasma/Containment>

using namespace Qt::StringLiterals;

RemoveAppletMenu::RemoveAppletMenu(Plasma::Containment *containment, QWidget *parent)
	: PanelMenu(QIcon::fromTheme(QStringLiteral("list-remove")),
				i18n("Remove From Panel"), containment, parent)
{
	if (containment) {
		connect(containment, &Plasma::Containment::appletsChanged, this, &PanelMenu::reinitialize);

		// Our QML contextmenu avoids displaying this menu if the panel is immutable, however
		// we should guard against it here just in case something in QML breaks.
		connect(containment, &Plasma::Applet::immutabilityChanged, this, &PanelMenu::reinitialize);
	}
}

void
RemoveAppletMenu::initialize()
{
	if (initialized()) return;
	
	// First clear the lists
	m_icons.clear();
	m_applets.clear();

	Plasma::Containment *containment = qobject_cast<Plasma::Containment *>(applet());
	if (!containment)
		return;

	const QList<Plasma::Applet *> allApplets = containment->applets();
	for (auto *applet : allApplets) {
		if (!applet)
			continue;

		// Avoid populating destroyed applets in to our list. Technically not needed since populateSubmenu() will
		// skip destroyed applet, but it's a good measure anyways!
		connect(applet, &Plasma::Applet::destroyedChanged, this, &PanelMenu::reinitialize, Qt::UniqueConnection);

		if (applet->destroyed())
			continue;
		
		// Icons get added to the icon list, applets get added to the applet list
		const QString pluginName = applet->pluginName();
		if (pluginName == "com.github.neeeeow.klassik.panel.icon"_L1 ||
			pluginName == "org.kde.plasma.icon"_L1)
			m_icons.append(applet);
		else
			m_applets.append(applet);
	}
}

void
RemoveAppletMenu::populate()
{
	if (!mayAddItem(AddToTarget::Panel)) {
		menuAction()->setEnabled(false);
		return;
	}
	
	// First create the submenus (which always appear!). Here we use regular QMenus
	QMenu *appletMenu = addMenu(i18n("Applet"));
	QMenu *iconMenu = addMenu(i18n("Icon"));

	// We disable the menus if no applets are present
	if (m_applets.isEmpty())
		appletMenu->menuAction()->setEnabled(false);
	else
		populateSubMenu(appletMenu, m_applets);
	
	if (m_icons.isEmpty())
		iconMenu->menuAction()->setEnabled(false);
	else	
		populateSubMenu(iconMenu, m_icons);

    menuAction()->setEnabled(appletMenu->menuAction()->isEnabled() || iconMenu->menuAction()->isEnabled());
}

void
RemoveAppletMenu::populateSubMenu(QMenu *menu, const QList<QPointer<Plasma::Applet>> &appletList) const
{
	if (!menu)
		return;

	for (const QPointer<Plasma::Applet> &applet : appletList) {
		// We must check if the applet has been destroyed, as when an applet is deletd, there
		// is the undo grace period, where the applet is marked as destroyed (and musn't be displayed)
		// in the panel, but appletsChanged hasn't fired yet.
		if (!applet || applet->destroyed())
			continue;
		QAction *action = menu->addAction(QIcon::fromTheme(applet->icon()), escapeMnemonics(applet->title()));
		connect(action, &QAction::triggered, this, [this, applet] {
			removeApplet(applet);
		});
	}

	// Provide the option to delete *all* applets in the list.
	if (menu->isEmpty()) {
		menu->menuAction()->setEnabled(false);
		return;
	} else
		menu->addSeparator();
	QAction *removeAllAction = menu->addAction(i18n("All"));
	connect(removeAllAction, &QAction::triggered, this, [this, appletList] {
		removeAllApplets(appletList);
	});
}

void
RemoveAppletMenu::removeApplet(Plasma::Applet *applet) const
{
	if (!applet || applet->immutable() || applet->destroyed())
		return;

	if (QAction *removeAction = applet->internalAction(QStringLiteral("remove")))
		removeAction->trigger();
	else
		applet->destroy();
}

void
RemoveAppletMenu::removeAllApplets(const QList<QPointer<Plasma::Applet>> &appletList) const
{
	if (!mayAddItem(AddToTarget::Panel))
		return;

	bool removedAny = false;
	for (const QPointer<Plasma::Applet> &applet : appletList) {
		if (applet && !applet->destroyed()) {
			applet->destroy();
			removedAny = true;
		}
	}

	if (!removedAny)
		return;

	auto *notification = new KNotification(QStringLiteral("plasmoidDeleted"));
	notification->setFlags(KNotification::SkipGrouping);
	notification->setComponentName(QStringLiteral("plasma_workspace"));
	notification->setIconName(QStringLiteral("edit-delete"));
	notification->setTitle(i18n("Widgets Removed"));
	notification->setText(i18n("All widgets have been removed from the panel."));
	notification->sendEvent();
}
