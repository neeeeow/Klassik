/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "removemenu.h"

#include <KLocalizedString>

#include <Plasma/Containment>

using namespace Qt::StringLiterals;

RemoveMenu::RemoveMenu(Plasma::Containment *containment, QWidget *parent)
	: PanelMenu(i18n("Remove From Panel"), containment, parent)
{
	if (containment)
		connect(containment, &Plasma::Containment::appletsChanged, this, &PanelMenu::reinitialize);
}

void
RemoveMenu::initialize()
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
RemoveMenu::populate()
{
	// First create the submenus (which always appear!). Here we use regular QMenus
	QMenu *appletMenu = addMenu(i18n("Applet"));
	QMenu *iconMenu = addMenu(i18n("Icon"));

	// We disable the menus if no applets are present (but still populate the remove all action, such that
	// it appears to contain a submenu. This is how KDE 3 did things.
	appletMenu->menuAction()->setEnabled(!m_applets.isEmpty());
	iconMenu->menuAction()->setEnabled(!m_icons.isEmpty());

	populateSubMenu(appletMenu, m_applets);
	populateSubMenu(iconMenu, m_icons);
}

void
RemoveMenu::populateSubMenu(QMenu *menu, const QList<QPointer<Plasma::Applet>> &appletList)
{
	if (!menu)
		return;

	for (Plasma::Applet *applet : appletList) {
		if (!applet)
			continue;
		QAction *action = menu->addAction(QIcon::fromTheme(applet->icon()), escapeMnemonics(applet->title()));
		connect(action, &QAction::triggered, applet, &Plasma::Applet::destroy);
	}

	// Provide the option to delete *all* applets in the list.
	if (!menu->isEmpty())
		menu->addSeparator();
	QAction *removeAllAction = menu->addAction(i18n("All"));
	connect(removeAllAction, &QAction::triggered, removeAllAction, [appletList]() {
		removeAllApplets(appletList);
	});
}

void
RemoveMenu::removeAllApplets(const QList<QPointer<Plasma::Applet>> &appletList)
{
	for (Plasma::Applet *applet: appletList)
		if (applet)
			applet->destroy();
}
