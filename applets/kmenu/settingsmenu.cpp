/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "settingsmenu.h"
#include "kmenuapplet.h"

#include <KLocalizedString>
#include <KService>

#include <PlasmaActivities/Stats/Query>

SettingsMenu::SettingsMenu(KMenuApplet *applet, QWidget *parent)
	: ServiceMenu(applet, parent)
{
	initialize();
}

SettingsMenu::SettingsMenu(const QString &title, KMenuApplet *applet, QWidget *parent)
	: ServiceMenu(title, applet, parent)
{
	initialize();
}

void
SettingsMenu::initialize()
{
	if (initialized()) return;
	ServiceMenu::initialize();

	using namespace KActivities::Stats;
	using namespace KActivities::Stats::Terms;

	auto query = AllResources | Agent(QStringLiteral("org.kde.systemsettings")) | HighScoredFirst | Limit(5);

	if (m_settingsList)
	    m_settingsList->deleteLater();
    m_settingsList = new ResultModel(query, this);	

	connectResultModel(m_settingsList, &SettingsMenu::updateSettingsMenu);

	updateSettingsMenu();

	setInitialized(true);
}

void
SettingsMenu::updateSettingsMenu()
{
	if (!m_settingsList) // sanity check
		return;
	
	clear();

	// Add settings action
	const KService::Ptr systemsettings = KService::serviceByDesktopName(QStringLiteral("systemsettings"));
	if (QAction *action = createActionFromService(systemsettings)) {
		addAction(action);
	}
	
	addSeparator();

	QList<QAction*> actionList;
	for (int i=0; i < m_settingsList->rowCount(); ++i) {
		QModelIndex index = m_settingsList->index(i,0);
		const QString storageId = QUrl(m_settingsList->data(index, KActivities::Stats::ResultModel::ResourceRole).toString()).path();
		KService::Ptr service = KService::serviceByStorageId(storageId);
		QAction *action = createActionFromService(service);
		if (action)
			actionList.append(action);
	}

	if (actionList.isEmpty()) {
		QAction *emptyAction = addAction(i18n("No Entries"));
		emptyAction->setEnabled(false);
	} else {
		addActions(actionList);
	}
}
