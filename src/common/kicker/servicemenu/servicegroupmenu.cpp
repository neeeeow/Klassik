/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "servicegroupmenu.h"

ServiceGroupMenu::ServiceGroupMenu(Plasma::Applet *applet, QWidget *parent)
	: ServiceGroupMenu(ItemInfo(KServiceGroup::root()), applet, parent) {}

ServiceGroupMenu::ServiceGroupMenu(const ItemInfo &info, Plasma::Applet *applet, QWidget *parent)
	: ServiceMenu(info, applet, parent), m_info(info)
{	
	// Set the search filter from the parent (if there is one!)
	if (const auto *parentMenu = qobject_cast<const ServiceGroupMenu *>(parentWidget())) {
		// Directly assign the variable here since there's no need to fire the signal
		m_searchFilter = parentMenu->searchFilter();
		// Set the filter based on the parent menu
		connect(parentMenu, &ServiceGroupMenu::searchFilterChanged, this, &ServiceGroupMenu::setSearchFilter);
	}

	// Connect search filter changed signal
	connect(this, &ServiceGroupMenu::searchFilterChanged, this, &ServiceGroupMenu::applySearchFilter);
}

void
ServiceGroupMenu::populate()
{
	addActions(createActionsFromServiceGroup());
	if (isEmpty())
		addAction(noEntriesAction());
}

QList<QAction *>
ServiceGroupMenu::createActionsFromServiceGroup()
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
			if (QAction *action = createActionFromItemInfo(ItemInfo(service))) {
				action->setEnabled(filterService(service, searchFilter()));
				actions.append(action);
			}
		} else if (entry->isType(KST_KServiceGroup)) {
			// If the entry is a service group, we need to make a submenu
			const KServiceGroup::Ptr subGroup(static_cast<KServiceGroup*>(entry.data()));
			if (!subGroup || !subGroup->isValid() || subGroup->noDisplay() || subGroup->childCount() == 0)
				continue;
			if (QAction *action = createActionFromItemInfo(ItemInfo(subGroup))) {
				action->setEnabled(filterServiceGroup(subGroup, searchFilter()));
				actions.append(action);
			}
		} else if (entry->isType(KST_KServiceSeparator))
			appendSeparatorActionToList(actions, this);
	}
	if (!actions.isEmpty() && actions.last()->isSeparator())
		delete actions.takeLast();
	return actions;
}

void
ServiceGroupMenu::applySearchFilter()
{	
	if (!fresh())
		return;
	
	const QList<QAction *> actions = actionsToFilter();
	if (actions.isEmpty()) return;
	
	filterActions(actions, searchFilter());
}

void
ServiceGroupMenu::filterActions(const QList<QAction *> &actions, QStringView filter)
{
	if (actions.isEmpty())
		return;
	
	for (QAction *action : actions) {	
		if (!action || action->isSeparator()) {
			continue;
		}
		const ItemInfo info = action->data().value<ItemInfo>();
		if (!info.isValid())
			continue;

		if (filter.isEmpty()) {
			action->setEnabled(true);
			continue;
		}
		bool enabled = false; // Whether or not to enable the individual action	   				
		if (const KService::Ptr service = info.service())
			enabled = filterService(service, filter);
		else if (const KServiceGroup::Ptr group = info.group())
			enabled = filterServiceGroup(group, filter);
		
		action->setEnabled(enabled);
	}
}

bool
ServiceGroupMenu::filterService(const KService::Ptr &service, QStringView filter)
{
	if (!service || !service->isValid() || service->noDisplay())
		return false;

	if (filter.isEmpty() ||
		service->name().contains(filter, Qt::CaseInsensitive) ||
		service->genericName().contains(filter, Qt::CaseInsensitive) ||
		service->untranslatedName().contains(filter, Qt::CaseInsensitive))
		return true;

	// We also check the keywords too
	for (const QString &keyword : service->keywords()) {
		if (keyword.contains(filter, Qt::CaseInsensitive))
			return true;
	}
	
	return false;
}

bool
ServiceGroupMenu::filterServiceGroup(const KServiceGroup::Ptr &group, QStringView filter)
{
	if (!group || !group->isValid() || group->noDisplay() || group->childCount() == 0)
		return false;

	if (filter.isEmpty())
		return true;

	// For the search filter, we can avoid sorting and ignore separators entirely
	const auto entries = group->entries(false, true, false);
	for (const auto &entry : entries) {
		if (entry->isType(KST_KService)) {
			// If the entry is a service, it's an individual application
			const KService::Ptr service(static_cast<KService*>(entry.data()));
			if (filterService(service, filter))
				return true;
		} else if (entry->isType(KST_KServiceGroup)) {
			const KServiceGroup::Ptr subGroup(static_cast<KServiceGroup*>(entry.data()));
			if (filterServiceGroup(subGroup, filter))
				return true;
		}
	}
	
	return false;
}
