#include "kmenu.h"
#include "popupmenutitle.h"

#include <QPoint>
#include <QRect>
#include <QDebug>

#include <KLocalizedString>
#include <KService>
#include <KIO/ApplicationLauncherJob>

#include <PlasmaActivities/Stats/Query>
#include <PlasmaActivities/ResourceInstance>

KMenu::KMenu(QWidget *parent)
	: QMenu(parent)
{
	initialize(); // Populate menu items
}

KMenu::~KMenu() = default;

void
KMenu::initialize()
{
	// Add the section headers
	m_recentHeader = new PopupMenuTitle(i18n("Recent Applications"), this);
	m_allAppsHeader = new PopupMenuTitle(i18n("All Applications"), this);
	m_actionsHeader = new PopupMenuTitle(i18n("Actions"), this);

	this->addAction(m_recentHeader);
	this->addAction(m_allAppsHeader);
	this->addAction(m_actionsHeader);
	
	createRecentMenuItems();
}

void
KMenu::createRecentMenuItems()
{
	using namespace KActivities::Stats;
	using namespace KActivities::Stats::Terms;
	
	// Run our query once.
	auto query = UsedResources
		| RecentlyUsedFirst
		| Url(QStringList{QStringLiteral("applications:*")})		
		| Agent::any()
		| Type::any()
		| Activity::any();

	m_recentApps = new ResultModel(query, this);

	// Whenever an application is launched, update the recent apps list
	connect(m_recentApps, &ResultModel::dataChanged, this, &KMenu::updateRecent);
	connect(m_recentApps, &ResultModel::modelReset, this, &KMenu::updateRecent);
	connect(m_recentApps, &ResultModel::rowsInserted, this, &KMenu::updateRecent);
	connect(m_recentApps, &ResultModel::rowsMoved, this, &KMenu::updateRecent);
	connect(m_recentApps, &ResultModel::rowsRemoved, this, &KMenu::updateRecent);

	// Update the list once to initially populate it
	updateRecent();
}

void
KMenu::updateRecent()
{	
	// First delete the actions from the menu
	for (QAction *action : m_recentActions) {
		this->removeAction(action);
	}

	// Clear out the list itself
	qDeleteAll(m_recentActions);
	m_recentActions.clear();

	using namespace KActivities::Stats;

	for (int i=0; i < 5; ++i) {
		QModelIndex index = m_recentApps->index(i,0);

		const QString storageId = m_recentApps->data(index, ResultModel::ResourceRole).toString().mid(QStringLiteral("applications:").length());
		KService::Ptr service = KService::serviceByStorageId(storageId);
		if (!service) // This shouldn't happen, but it might
			continue;

		// Create the menu item itself
		QAction *action = new QAction(QIcon::fromTheme(service->icon()), service->name(), this);
		connect(action, &QAction::triggered, this, [service]() {
			auto *job = new KIO::ApplicationLauncherJob(service);
			job->start();

			KActivities::ResourceInstance::notifyAccessed(
			    QUrl(QStringLiteral("applications:") + service->storageId()),
				QStringLiteral("com.github.neeeeow.klassik.kmenu")
				);			
		});

	    this->insertAction(m_allAppsHeader, action);
		m_recentActions.append(action);
	}
}
