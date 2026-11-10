/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "servicemenu.h"
#include "servicemenuapplet.h"

#include <KFileItem>

ServiceMenu::ServiceMenu(ServiceMenuApplet *applet, QWidget *parent)
: PanelMenu(applet, parent) {}

ServiceMenu::ServiceMenu(const QString &title, ServiceMenuApplet *applet, QWidget *parent)
: PanelMenu(title, applet, parent) {}

ServiceMenu::ServiceMenu(const QIcon &icon, const QString &title, ServiceMenuApplet *applet, QWidget *parent)
	: PanelMenu(icon, title, applet, parent) {}

void
ServiceMenu::initialize()
{
	if (initialized()) return;
	if (auto a = applet()) {	
		m_displayGenericName = a->getConfigValue<bool>(QStringLiteral("showGenericName"), m_displayGenericName);
		setToolTipsVisible(a->getConfigValue<bool>(QStringLiteral("showTooltips"), toolTipsVisible()));
	}
}

ServiceMenuApplet*
ServiceMenu::applet() const
{
	return qobject_cast<ServiceMenuApplet *>(PanelMenu::applet());
}

QAction*
ServiceMenu::createActionFromService(const KService::Ptr &service, const QUrl &url)
{
	if (!service || !service->isValid())
		return nullptr;

	QString title = escapeMnemonics(service->name());
	if (m_displayGenericName) {
		const QString genericName = escapeMnemonics(service->genericName());
		if (!genericName.isEmpty() && genericName != title)
			title.append(QStringLiteral(" (%1)").arg(genericName));
	}
	
	auto *action = new QAction(
		QIcon::fromTheme(service->icon(),
						 QIcon::fromTheme(QStringLiteral("application-x-executable"))),
		title, this);

	action->setData(QUrl::fromLocalFile(service->entryPath())); // Store the URL to the .desktop entry
	action->setToolTip(service->comment()); // Set the action tooltip
	
	connect(action, &QAction::triggered, action, [service, url]() {
		execService(service, url);
	});

	return action;
}

QAction*
ServiceMenu::createActionFromUrl(const QUrl &url)
{
	if (!url.isValid())
		return nullptr;	

	KFileItem fileItem(url);
	const QIcon icon = QIcon::fromTheme(fileItem.iconName(), QIcon::fromTheme(QStringLiteral("text-x-generic")));

	return createActionFromUrl(url, icon, fileItem.text());
}

QAction*
ServiceMenu::createActionFromUrl(const QUrl &url, const QIcon &icon, const QString &title)
{
	if (!url.isValid())
		return nullptr;

	auto *action = new QAction(icon, escapeMnemonics(title), this);
	action->setData(url);
	action->setToolTip(url.toDisplayString(QUrl::PreferLocalFile));
	
	connect(action, &QAction::triggered, action, [url]() {
	    execUrl(url);
	});

	return action;
}
