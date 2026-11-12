/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "addappsmenu.h"

#include <KLocalizedString>

AddAppsMenu::AddAppsMenu(Plasma::Applet *applet, QWidget *parent)
	: AddAppsMenu(QString(), applet, parent) {}

AddAppsMenu::AddAppsMenu(const QString &relPath, Plasma::Applet *applet, QWidget *parent)
	: PanelMenu(applet, parent), m_relPath(relPath)
{
	// Set the title/icon
	const KServiceGroup::Ptr group = KServiceGroup::group(m_relPath);
	if (group && group->isValid()) {
		if (m_relPath.isEmpty()) {
			qWarning() << "EMPTY REL PATH!";
			setTitle(i18n("Applications"));
			setIcon(QIcon::fromTheme(QStringLiteral("applications-other")));
		} else {
			setTitle(escapeMnemonics(group->caption()));
			setIcon(QIcon::fromTheme(group->icon(), QIcon::fromTheme(QStringLiteral("applications-other"))));
		}
	}
}

void
AddAppsMenu::populate()
{
	// Add this menu option
	qWarning() << icon().name();
	
	if (mayAddItem(AddToTarget::Panel)) {
		QAction *addThisMenu = addAction(icon(), i18n("Add This Menu"));
		connect(addThisMenu, &QAction::triggered, this, [this]() {
			if (!addLauncher(AddToTarget::Panel, urlFromServiceGroup(m_relPath)))
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
	const KServiceGroup::Ptr group = KServiceGroup::group(m_relPath);
	if (!group || !group->isValid())
		return actions;

	const auto entries = group->entries(true);
	for (const auto &entry : entries) {
		if (entry->isType(KST_KService)) {
			// If the entry is a service, it's an individual application
			const KService::Ptr service(static_cast<KService*>(entry.data()));
			if (!service || !service->isValid())
				continue;

			const QUrl &url = QUrl::fromLocalFile(service->entryPath());
			
			auto *action = new QAction(
				QIcon::fromTheme(service->icon(),
								 QIcon::fromTheme(QStringLiteral("application-x-executable"))),
				escapeMnemonics(service->name()), this);
			action->setData(url);
			
			connect(action, &QAction::triggered, this, [this, url]() {
				if (!addLauncher(AddToTarget::Panel, url))
					qWarning() << "failed to add launcher to panel!";
			});

			actions.append(action);
		} else if (entry->isType(KST_KServiceGroup)) {
			// If the entry is a service group, we need to make a submenu
			const KServiceGroup::Ptr subGroup(static_cast<KServiceGroup*>(entry.data()));
			if (!subGroup || !subGroup->isValid() || subGroup->childCount() == 0)
				continue;

			auto *subMenu = new AddAppsMenu(subGroup->relPath(), applet(), this);
			auto *action = new QAction(subMenu->icon(), subMenu->title(), this);
			action->setMenu(subMenu);
			action->setData(urlFromServiceGroup(subGroup));
			actions.append(action);
		}
	}
	return actions;
}
