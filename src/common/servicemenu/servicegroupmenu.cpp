/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "servicegroupmenu.h"

#include <KLocalizedString>

ServiceGroupMenu::ServiceGroupMenu(ServiceMenuApplet *applet, QWidget *parent)
	: ServiceGroupMenu(KServiceGroup::root(), applet, parent) {}

ServiceGroupMenu::ServiceGroupMenu(const KServiceGroup::Ptr &group, ServiceMenuApplet *applet, QWidget *parent)
	: ServiceMenu(applet, parent), m_group(group)
{
	// Set the KServiceGroup to be used
	if (m_group && m_group != KServiceGroup::root()) {
		setTitle(escapeMnemonics(m_group->caption()));
		setIcon(QIcon::fromTheme(m_group->icon(), QIcon::fromTheme(QStringLiteral("applications-other"))));
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
	if (!m_group || !m_group->isValid())
		return;
	
	addActions(createActionsFromServiceGroup(m_group));

	if (isEmpty()) {
		QAction *emptyAction = addAction(i18n("No Entries"));
		emptyAction->setEnabled(false);
	}
}

QList<QAction *>
ServiceGroupMenu::createActionsFromServiceGroup(const KServiceGroup::Ptr &group)
{
	QList<QAction *> actions;
	if (!group || !group->isValid())
		return actions;

	const auto entries = group->entries(true);
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
			if (subGroup->childCount() == 0)
				continue;

			auto *subMenu = new ServiceGroupMenu(subGroup, applet(), this);
			auto *action = new QAction(subMenu->icon(), subMenu->title(), this);
			action->setMenu(subMenu);
			action->setData(QVariant::fromValue(subGroup));
			action->setEnabled(filterServiceGroup(subGroup, searchFilter()));
			actions.append(action);
		}
	}
	return actions;
}

void
ServiceGroupMenu::applySearchFilter()
{
	if (!initialized() || isEmpty() || !isVisible())
		return;

	for (QAction *action : actions()) {
		// Set the enabled/disabled state of the action
		filterAction(action);
	}
}

void
ServiceGroupMenu::filterAction(QAction *action) const
{
	if (!action || action->isSeparator())
		return;

	const QString &filter = searchFilter();
	if (filter.isEmpty()) {
		// If the filter is empty, enable the action and bail.
		action->setEnabled(true);
		return;
	}

	bool enabled = false;
	const QVariant data = action->data();	
	if (data.canConvert<KService::Ptr>()) {
		// If it's a KService, just filter based on the name
		enabled = filterService(data.value<KService::Ptr>(), filter);
	} else if (data.canConvert<KServiceGroup::Ptr>()) {
		enabled = filterServiceGroup(data.value<KServiceGroup::Ptr>(), filter);
	}

	action->setEnabled(enabled);
}

bool
ServiceGroupMenu::filterService(const KService::Ptr &service, QStringView filter)
{
	if (!service || !service->isValid())
		return false;

	if (filter.isEmpty() || service->name().contains(filter, Qt::CaseInsensitive)
		|| service->genericName().contains(filter, Qt::CaseInsensitive))
		return true;
	
	return false;
}

bool
ServiceGroupMenu::filterServiceGroup(const KServiceGroup::Ptr &group, QStringView filter)
{
	if (!group || !group->isValid() || group->childCount() == 0)
		return false;

	const auto entries = group->entries(true);
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
