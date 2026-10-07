/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "recentdocsmenu.h"
#include "kmenuapplet.h"

#include <KLocalizedString>

#include <PlasmaActivities/Stats/Query>

RecentDocsMenu::RecentDocsMenu(KMenuApplet *applet, QWidget *parent)
	: ServiceMenu(applet, parent)
{
	initialize();
}

RecentDocsMenu::RecentDocsMenu(const QString &title, KMenuApplet *applet, QWidget *parent)
	: ServiceMenu(title, applet, parent)
{
	initialize();
}

void
RecentDocsMenu::initialize()
{
	if (initialized()) return;
	ServiceMenu::initialize();

	// Setup the recent documents query
	// NB: we deviate from the KDE 3 era code here, since using KRecentDocument doesn't
	// work very well.
	using namespace KActivities::Stats;
	using namespace KActivities::Stats::Terms;
	
	// Run our query once.
	const int numRecentDocs = applet() ? applet()->getConfigValue<int>(QStringLiteral("numRecentDocs")) : 15;
	const auto query = UsedResources
		| RecentlyUsedFirst
		| Agent::any()
		| Type::files()
		| Activity::current()
		| Url::file()
		| Limit(numRecentDocs > 0 ? numRecentDocs : 1);

	if (m_fileList)
		m_fileList->deleteLater();
    m_fileList = new ResultModel(query, this);	

	// Whenever an application is launched, update the recent apps list
	connectResultModel(m_fileList, &RecentDocsMenu::updateRecentDocs);

	updateRecentDocs();

	setInitialized(true);
}

void
RecentDocsMenu::updateRecentDocs()
{
	if (!m_fileList) // sanity check
		return;
	
    clear();	
	QAction *clearAction = addAction(QIcon::fromTheme(QStringLiteral("edit-clear-history")), i18n("Clear History"));
	connect(clearAction, &QAction::triggered, m_fileList, &KActivities::Stats::ResultModel::forgetAllResources, Qt::QueuedConnection);
	addSeparator();

	QList<QAction *> actionList;
	for (int i=0; i < m_fileList->rowCount(); ++i) {
		const QModelIndex index = m_fileList->index(i,0);
		const QUrl url = QUrl::fromUserInput(m_fileList->data(index, KActivities::Stats::ResultModel::ResourceRole).toString());
		QAction *action = createActionFromUrl(url);
		if (!action)
			continue;
		actionList.append(action);
	}

	if (actionList.isEmpty()) {
		QAction *emptyAction = addAction(i18n("No Entries"));
		emptyAction->setEnabled(false);
	} else {
		addActions(actionList);
	}

}
