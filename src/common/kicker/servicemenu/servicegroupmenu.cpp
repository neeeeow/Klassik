/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "servicegroupmenu.h"

ServiceGroupMenu::ServiceGroupMenu(ServiceMenuApplet *applet, QWidget *parent)
	: ServiceGroupMenu(KServiceGroup::root(), applet, parent) {}

ServiceGroupMenu::ServiceGroupMenu(const KServiceGroup::Ptr &group, ServiceMenuApplet *applet, QWidget *parent)
	: ServiceMenu(applet, parent)
{
	if (group && group->isValid()) {
		setTitle(escapeMnemonics(group->caption()));
		setIcon(QIcon::fromTheme(group->icon(), QIcon::fromTheme(QStringLiteral("applications-other"))));
		m_relPath = group->relPath();
	}

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
	const KServiceGroup::Ptr group = KServiceGroup::group(m_relPath);
	if (!group || !group->isValid())
		return actions;

	const auto entries = group->entries(true, true, true);
	for (const auto &entry : entries) {
		if (entry->isType(KST_KService)) {
			// If the entry is a service, it's an individual application
			const KService::Ptr service(static_cast<KService*>(entry.data()));
			if (QAction *action = createActionFromService(service)) {
				action->setEnabled(filterService(service, searchFilter()));
				actions.append(action);
			}
		} else if (entry->isType(KST_KServiceGroup)) {
			// If the entry is a service group, we need to make a submenu
			const KServiceGroup::Ptr subGroup(static_cast<KServiceGroup*>(entry.data()));
			if (!subGroup || !subGroup->isValid() || subGroup->childCount() == 0)
				continue;

			auto *subMenu = new ServiceGroupMenu(subGroup, applet(), this);
			auto *action = subMenu->menuAction();
			action->setData(urlFromServiceGroup(subGroup));
			action->setEnabled(filterServiceGroup(subGroup, searchFilter()));
			actions.append(action);
		} else if (entry->isType(KST_KServiceSeparator))
			appendSeparatorActionToList(actions, this);
	}
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
		const QUrl url = action->data().toUrl();
		if (!url.isValid())
			continue;
		if (filter.isEmpty()) {
			action->setEnabled(true);
			continue;
		}
		bool enabled = false; // Whether or not to enable the individual action	   				
		if (const KService::Ptr service = serviceFromUrl(url))
			enabled = filterService(service, filter);
		else if (const KServiceGroup::Ptr group = serviceGroupFromUrl(url))
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
	if (!group || !group->isValid())
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
