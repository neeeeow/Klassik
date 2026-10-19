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
	if (m_group && m_group != KServiceGroup::root()) {
		setTitle(escapeMnemonics(m_group->caption()));
		setIcon(QIcon::fromTheme(m_group->icon(), QIcon::fromTheme(QStringLiteral("applications-other"))));
	}
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
			if (QAction *action = createActionFromService(service))
				actions.append(action);
		} else if (entry->isType(KST_KServiceGroup)) {
			// If the entry is a service group, we need to make a submenu
			const KServiceGroup::Ptr subGroup(static_cast<KServiceGroup*>(entry.data()));
			if (subGroup->childCount() == 0)
				continue;

			auto *subMenu = new ServiceGroupMenu(subGroup, applet(), this);
			auto *action = new QAction(subMenu->icon(), subMenu->title(), this);
			action->setMenu(subMenu);
			action->setData(QVariant::fromValue(subGroup));
			actions.append(action);
		}
	}
	return actions;
}
