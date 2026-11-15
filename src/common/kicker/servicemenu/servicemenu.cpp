/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "servicemenu.h"
#include "iteminfo.h"
#include "dirmenu.h"
#include "servicegroupmenu.h"

ServiceMenu::ServiceMenu(Plasma::Applet *applet, QWidget *parent)
	: PanelMenu(applet, parent) {}

ServiceMenu::ServiceMenu(const ItemInfo &info, Plasma::Applet *applet, QWidget *parent)
	: PanelMenu(info, applet, parent) {}

ServiceMenu::ServiceMenu(const QString &title, Plasma::Applet *applet, QWidget *parent)
	: PanelMenu(title, applet, parent) {}

ServiceMenu::ServiceMenu(const QIcon &icon, const QString &title, Plasma::Applet *applet, QWidget *parent)
	: PanelMenu(icon, title, applet, parent) {}

void
ServiceMenu::initialize()
{
	if (initialized()) return;

	m_showGenericName = getConfigValue<bool>(QStringLiteral("showGenericName"), m_showGenericName);
	setToolTipsVisible(getConfigValue<bool>(QStringLiteral("showTooltips"), toolTipsVisible()));
}

QAction*
ServiceMenu::createActionFromItemInfo(const ItemInfo &info)
{
	QAction *action = nullptr;
	
	switch (info.type()) {
	case ItemInfo::Type::File: {
	    action = new QAction(QIcon::fromTheme(info.icon()), escapeMnemonics(info.name()), this);
		connect(action, &QAction::triggered, action, [url = info.url()]() {
			execUrl(url);
		});		
	    break;
	}
	case ItemInfo::Type::Directory: {
		auto *subMenu = new DirMenu(info, applet(), this);
	    action = subMenu->menuAction();

	    break;
	}
	case ItemInfo::Type::Service: {		
		QString title = escapeMnemonics(info.name());
		if (m_showGenericName) {
			const QString genericName = escapeMnemonics(info.genericName());
			if (!genericName.isEmpty() && genericName != title)
				title.append(QStringLiteral(" (%1)").arg(genericName));
		}
		
	    action = new QAction(QIcon::fromTheme(info.icon()), title, this);
		connect(action, &QAction::triggered, action, [service = info.service()]() {
			execService(service);
		});
		
		break;
	}
	case ItemInfo::Type::ServiceGroup: {				
		auto *subMenu = new ServiceGroupMenu(info, applet(), this);
	    action = subMenu->menuAction();
	    break;
	}
	case ItemInfo::Type::Invalid: {
		qWarning() << "ServiceMenu::createActionFromItemInfo: invalid ItemInfo!";
	    break;
	}
	}

	if (action) {
		action->setToolTip(info.comment());
		action->setData(QVariant::fromValue(info));
	}
	return action;
}
