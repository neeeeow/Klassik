#include "kmenu.h"
#include "popupmenutitle.h"

#include <QPoint>
#include <QRect>
#include <QDebug>

#include <KLocalizedString>
#include <KService>
#include <KServiceGroup>
#include <KSycoca>
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
	m_recentHeader = new PopupMenuTitle(i18n("Most Used Applications"), this);
	m_allAppsHeader = new PopupMenuTitle(i18n("All Applications"), this);
	m_actionsHeader = new PopupMenuTitle(i18n("Actions"), this);

	this->addAction(m_recentHeader);
	this->addAction(m_allAppsHeader);
	this->addAction(m_actionsHeader);

	createRecentMenuItems();

	connect(KSycoca::self(), &KSycoca::databaseChanged, this, &KMenu::updateApplications); // Update applications menu if it changes
	updateApplications();
}

void
KMenu::createRecentMenuItems()
{
	using namespace KActivities::Stats;
	using namespace KActivities::Stats::Terms;
	
	// Run our query once.
	auto query = UsedResources
		| HighScoredFirst
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
	/* Updates the most used applications name. It probably *shouldn't* be called updateRecent,
	   but we follow the convention used by the original KDE 3 KMenu, even if it doesn't
	   make sense */
	
	// Cleanup the old menu items
	cleanupActionList(m_recentActions);

	using namespace KActivities::Stats;

	for (int i=0; i < 3; ++i) {
		QModelIndex index = m_recentApps->index(i,0);

		const QString storageId = m_recentApps->data(index, ResultModel::ResourceRole).toString().mid(QStringLiteral("applications:").length());
		KService::Ptr service = KService::serviceByStorageId(storageId);
		if (!service) // This shouldn't happen, but it might
			continue;

		// Create the menu item itself. Note, we use .replace(QStringLiteral("&"), QStringLiteral("&&")) to ensure that ampersands
		// don't inadvertently get interpreted as mnemonics. There is probably an easier way to do this, but it works!
		QAction *action = new QAction(QIcon::fromTheme(service->icon()), service->name().replace(QStringLiteral("&"), QStringLiteral("&&")), this);
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

void
KMenu::updateApplications()
{
	// Cleanup old items
	cleanupActionList(m_applicationActions);
	
	// The root of the applications menu
    KServiceGroup::Ptr root = KServiceGroup::root();

	if (!root || !root->isValid()) // sanity check
		return;

	// Define a recursive lambda for traversing the service groups and populating the submenus
	std::function<void(QMenu *, KServiceGroup::Ptr)> populateSubmenu =
		[&](QMenu *parent, KServiceGroup::Ptr group) {
			for (const auto &entry : group->entries(true)) {
				if (entry->isType(KST_KService)) {
					// If the entry is a service, it's an individual application
				    KService::Ptr service(static_cast<KService*>(entry.data()));

					// Create the entry
					QAction *action = new QAction(QIcon::fromTheme(service->icon()), service->name().replace(QStringLiteral("&"), QStringLiteral("&&")), this);
					connect(action, &QAction::triggered, this, [service]() {
						auto *job = new KIO::ApplicationLauncherJob(service);
						job->start();
						KActivities::ResourceInstance::notifyAccessed(
							QUrl(QStringLiteral("applications:") + service->storageId()),
							QStringLiteral("com.github.neeeeow.klassik.kmenu")
							);			
					});

					if (group == root) {
						this->insertAction(m_actionsHeader, action);
						m_applicationActions.append(action);
					} else
						parent->addAction(action);
				} else if (entry->isType(KST_KServiceGroup)) {
					// If the entry is a service group, we need to make a submenu and recurse through this function
				    KServiceGroup::Ptr subGroup(static_cast<KServiceGroup*>(entry.data()));
					if (subGroup->childCount() == 0)
						continue;
					
					QMenu *subMenu = new QMenu(subGroup->caption().replace(QStringLiteral("&"), QStringLiteral("&&")), this);
					subMenu->setIcon(QIcon::fromTheme(subGroup->icon()));

					if (group == root) {
						// If the group is at the root, we insert the submenu in the main menu and keep track of it in our list
						QAction *action = this->insertMenu(m_actionsHeader, subMenu);
						m_applicationActions.append(action);
					} else {
						parent->addMenu(subMenu);
					}

					populateSubmenu(subMenu, subGroup);
				}
			}
			
		};

	populateSubmenu(this, root);
}

void
KMenu::cleanupActionList(QList<QAction *> &actionList)
{
	// Cleans up all member actions of our QList from the menu
	for (QAction *action : actionList) {
		this->removeAction(action);
	}

	// Clear out the list itself
	qDeleteAll(actionList);
	actionList.clear();
}
