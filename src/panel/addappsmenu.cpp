/*
  SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "addappsmenu.h"

#include <KLocalizedString>

AddAppsMenu::AddAppsMenu(Plasma::Applet *applet, QWidget *parent)
	: AddAppsMenu(ItemInfo(KServiceGroup::root()), applet, parent) {}

AddAppsMenu::AddAppsMenu(const ItemInfo &info, Plasma::Applet *applet, QWidget *parent)
	: PanelMenu(info, applet, parent), m_info(info) {}

void
AddAppsMenu::populate()
{
	// Add this menu option	
	if (mayAddItem(AddToTarget::Panel)) {
		QAction *addThisMenu = addAction(icon(), i18n("Add This Menu"));
		connect(addThisMenu, &QAction::triggered, this, [this] {
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
	if (!group || !mayAddItem(AddToTarget::Panel))
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
			action->setToolTip(info.comment());
			action->setData(QVariant::fromValue(info));
			connect(action, &QAction::triggered, this, [this, url = info.url()] {
				if (!addLauncher(AddToTarget::Panel, url))
					qWarning() << "failed to add launcher to panel!";
			});
			actions.append(action);
		} else if (entry->isType(KST_KServiceGroup)) {
			// If the entry is a service group, we need to make a submenu
			const KServiceGroup::Ptr subGroup(static_cast<KServiceGroup*>(entry.data()));
			if (!subGroup || !subGroup->isValid() || subGroup->noDisplay() || subGroup->childCount() == 0)
				continue;

			const ItemInfo info(subGroup);
			if (!info.isValid())
				continue;
			
			auto *subMenu = new AddAppsMenu(info, applet(), this);
			QAction *action = subMenu->menuAction();
			action->setToolTip(info.comment());
			action->setData(QVariant::fromValue(info));
			actions.append(action);
		} else if (entry->isType(KST_KServiceSeparator))
			appendSeparatorActionToList(actions, this);
	}
	
	if (!actions.isEmpty() && actions.last()->isSeparator())
		delete actions.takeLast();
	return actions;
}
