/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "servicemenu.h"

ServiceMenu::ServiceMenu(ServiceMenuApplet *applet, QWidget *parent)
	: ServiceMenu(QIcon(), QString(), applet, parent) {}

ServiceMenu::ServiceMenu(const QString &title, ServiceMenuApplet *applet, QWidget *parent)
	: ServiceMenu(QIcon(), title, applet, parent) {}

ServiceMenu::ServiceMenu(const QIcon &icon, const QString &title, ServiceMenuApplet *applet, QWidget *parent)
	: ServiceMenuBase(icon, title, applet, parent)
{
	connect(this, &QMenu::aboutToShow, this, &ServiceMenu::refreshContents, Qt::UniqueConnection);
}

void
ServiceMenu::initialize()
{
	if (m_initialized) return;
	loadConfig();
	rebuild();	
	m_initialized = true;
}

void
ServiceMenu::reinitialize()
{
	if (!m_initialized)
		return;
	m_initialized = false;
}

void
ServiceMenu::refreshContents()
{
	if (!m_initialized)
		initialize();
	clear();
	populate();
}
