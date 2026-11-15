/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "addappsmenu.h"

#include <KLocalizedString>

AddAppsMenu::AddAppsMenu(Plasma::Applet *applet, QWidget *parent)
	: AddAppsMenu(KServiceGroup::root(), applet, parent) {}

AddAppsMenu::AddAppsMenu(const KServiceGroup::Ptr &group, Plasma::Applet *applet, QWidget *parent)
	: PanelMenu(applet, parent), m_info(group)
{
	// Set the title/icon
	setTitle(escapeMnemonics(m_info.name()));
	setIcon(QIcon::fromTheme(m_info.icon()));
}

void
AddAppsMenu::populate()
{
	// Add this menu option	
	if (mayAddItem(AddToTarget::Panel)) {
		QAction *addThisMenu = addAction(icon(), i18n("Add This Menu"));
		connect(addThisMenu, &QAction::triggered, this, [this]() {
			if (!addLauncher(AddToTarget::Panel, m_info.url()))
				qWarning() << "failed to add launcher to panel!";
		});
		addSeparator();
	}

	// Populate the rest of the menu
	auto actions = createActionsFromServiceGroup();
	if (!actions.isEmpty())
		addActions(actions);
	else
		addAction(noEntriesAction());
}

QList<QAction *>
AddAppsMenu::createActionsFromServiceGroup()
{
	QList<QAction *> actions;
	m_info.refresh(); // Ensure we store an up-to-date ServiceGroup	
	const KServiceGroup::Ptr group = m_info.group();
	if (!group)
		return actions;

	const auto entries = group->entries(true, true, true);
	for (const auto &entry : entries) {
		if (entry->isType(KST_KService)) {
			// If the entry is a service, it's an individual application
			const KService::Ptr service(static_cast<KService*>(entry.data()));
			if (!service || service->noDisplay())
				continue;

			const ItemInfo info(service);
			if (!info.isValid())
				continue;
			
			auto *action = new QAction(QIcon::fromTheme(info.icon()), escapeMnemonics(info.name()), this);			
			action->setData(QVariant::fromValue(info));

			connect(action, &QAction::triggered, this, [this, info]() {
				if (!addLauncher(AddToTarget::Panel, info.url()))
					qWarning() << "failed to add launcher to panel!";
			});
		} else if (entry->isType(KST_KServiceGroup)) {
			// If the entry is a service group, we need to make a submenu
			const KServiceGroup::Ptr subGroup(static_cast<KServiceGroup*>(entry.data()));
			if (!subGroup || !subGroup->isValid() || subGroup->childCount() == 0)
				continue;

			auto *subMenu = new AddAppsMenu(subGroup, applet(), this);
			auto *action = subMenu->menuAction();
			action->setData(QVariant::fromValue(subMenu->info()));
			actions.append(action);
		} else if (entry->isType(KST_KServiceSeparator))
			appendSeparatorActionToList(actions, this);
	}
	
	return actions;
}
