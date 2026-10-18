/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "servicegroupmenu.h"

#include <KLocalizedString>

ServiceGroupMenu::ServiceGroupMenu(const KServiceGroup::Ptr &group, ServiceMenuApplet *applet, QWidget *parent)
	: ServiceMenu(applet, parent), m_group(group)
{
	initialize();
}

void
ServiceGroupMenu::rebuild()
{
	if (!m_group || !m_group->isValid())
		return;

	setTitle(escapeMnemonics(m_group->caption()));
	setIcon(QIcon::fromTheme(m_group->icon(), QIcon::fromTheme(QStringLiteral("applications-other"))));
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

void
ServiceGroupMenu::setServiceGroup(const KServiceGroup::Ptr &group)
{
	m_group = group;
	reinitialize();
}
