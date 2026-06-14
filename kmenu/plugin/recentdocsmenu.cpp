#include "recentdocsmenu.h"

#include <KLocalizedString>

#include <PlasmaActivities/Stats/Query>

RecentDocsMenu::RecentDocsMenu(Plasma::Containment *containment, QWidget *parent)
	: ServiceMenu(containment, parent)
{
	initialize();
}

RecentDocsMenu::RecentDocsMenu(const QString &title, Plasma::Containment *containment, QWidget *parent)
	: ServiceMenu(title, containment, parent)
{
	initialize();
}

RecentDocsMenu::~RecentDocsMenu() = default;

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
	auto query = UsedResources
		| RecentlyUsedFirst
		| Agent::any()
		| Type::files()
		| Activity::current()
		| Url::file();

    m_fileList = new ResultModel(query, this);	

	// Whenever an application is launched, update the recent apps list
	connect(m_fileList, &ResultModel::dataChanged, this, &RecentDocsMenu::updateRecentDocs);
	connect(m_fileList, &ResultModel::modelReset, this, &RecentDocsMenu::updateRecentDocs);
	connect(m_fileList, &ResultModel::rowsInserted, this, &RecentDocsMenu::updateRecentDocs);
	connect(m_fileList, &ResultModel::rowsMoved, this, &RecentDocsMenu::updateRecentDocs);
	connect(m_fileList, &ResultModel::rowsRemoved, this, &RecentDocsMenu::updateRecentDocs);

	updateRecentDocs();

	setInitialized(true);
}

void
RecentDocsMenu::updateRecentDocs()
{
	if (!m_fileList) // sanity check
		return;
	
	clear();
	QAction *clearAction = addAction(QIcon::fromTheme(QStringLiteral("history-clear")), i18n("Clear History"));
	connect(clearAction, &QAction::triggered, m_fileList, &KActivities::Stats::ResultModel::forgetAllResources);
	addSeparator();

	QList<QUrl> urlList;
	for (int i=0; i < qMin(15, m_fileList->rowCount()); ++i) {
		QModelIndex index = m_fileList->index(i,0);
		QUrl url = QUrl::fromUserInput(m_fileList->data(index, KActivities::Stats::ResultModel::ResourceRole).toString());
		if (!url.isValid())
			continue;
		urlList.append(url);
	}

	if (urlList.isEmpty()) {
		QAction *emptyAction = addAction(i18n("No Entries"));
		emptyAction->setEnabled(false);
		return;
	}

	for (QUrl url : urlList) {
		QAction *action = createActionFromUrl(url);
		if (!action)
			continue;
		addAction(action);
	}
}
