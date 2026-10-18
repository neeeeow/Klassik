/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "recentdocsmenu.h"
#include "servicemenuapplet.h"

#include <QFileInfo>

#include <KLocalizedString>

#include <PlasmaActivities/Stats/ResultSet>
#include <PlasmaActivities/Stats/Query>
#include <PlasmaActivities/Stats/Cleaning>

using namespace KActivities::Stats;
using namespace KActivities::Stats::Terms;

RecentDocsMenu::RecentDocsMenu(ServiceMenuApplet *applet, QWidget *parent)
	: RecentDocsMenu(QIcon(), QString(), applet, parent) {}

RecentDocsMenu::RecentDocsMenu(const QString &title, ServiceMenuApplet *applet, QWidget *parent)
	: RecentDocsMenu(QIcon(), title, applet, parent) {}

RecentDocsMenu::RecentDocsMenu(const QIcon &icon, const QString &title, ServiceMenuApplet *applet, QWidget *parent)
	: ServiceMenu(icon, title, applet, parent)
{
	initialize();
}

void
RecentDocsMenu::populate()
{
	// Add clear recent documents action
	QAction *clearAction = addAction(QIcon::fromTheme(QStringLiteral("edit-clear-history")), i18n("Clear History"));
	connect(clearAction, &QAction::triggered, this, &RecentDocsMenu::forgetDocuments, Qt::QueuedConnection);
	addSeparator();
	
	// Actions to add to the menu
	QList<QAction *> actions;
	
	// Setup the recent documents query
	// NB: we deviate from the KDE 3 era code here, since using KRecentDocument doesn't
	// work very well.
	const int numRecentDocs = applet() ? applet()->getConfigValue<int>(QStringLiteral("numRecentDocs")) : 15;
	const auto query = UsedResources
		| RecentlyUsedFirst
		| Agent::any()
		| Type::files()
		| Activity::current()
		| Url::file();

	ResultSet results(query);
	ResultSet::const_iterator resultIt;
	resultIt = results.begin();

	while (actions.count() < numRecentDocs && resultIt != results.end()) {
		const QUrl url = (*resultIt).url();
		++resultIt;

		if (!url.isLocalFile() || !QFileInfo::exists(url.toLocalFile()))
			continue;
		
		QAction *action = createActionFromUrl(url);
		if (!action)
			continue;
		actions.append(action);
	}

	if (actions.isEmpty()) {
		QAction *emptyAction = addAction(i18n("No Entries"));
		emptyAction->setEnabled(false);
		clearAction->setEnabled(false);
	} else
		addActions(actions);
}

void
RecentDocsMenu::forgetDocuments() const
{
	const auto query = UsedResources
		| Agent::any()
		| Type::files()
		| Activity::current()
		| Url::file();
	KActivities::Stats::forgetResources(query);
}
