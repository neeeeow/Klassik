#include "kmenu.h"
#include "popupmenutitle.h"

#include <QPoint>
#include <QRect>
#include <QHBoxLayout>
#include <QToolButton>
#include <QShortcut>

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
	this->installEventFilter(this);
	initialize(); // Populate menu items
}

KMenu::~KMenu() = default;

bool
KMenu::eventFilter(QObject *object, QEvent *event)
{
	// Give the search bar focus as soon as the mouse enters it
	if (object == m_searchLineEdit) {
		if (event->type() == QEvent::Enter) {
			if (QWidget *widget = qobject_cast<QWidget *>(object))
				widget->setFocus();
		} else if (event->type() == QEvent::Leave)
			this->setFocus();
	}

	return QMenu::eventFilter(object, event);
}

void
KMenu::initialize()
{
	// Add the section headers
	m_allAppsHeader = new PopupMenuTitle(i18n("All Applications"), this);
	m_actionsHeader = new PopupMenuTitle(i18n("Actions"), this);

	this->addAction(m_allAppsHeader);
	this->addAction(m_actionsHeader);

	createRecentMenuItems();

	createApplicationsItems();
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
	
	cleanupActionList(m_recentActions);

	using namespace KActivities::Stats;

	if (m_recentApps->rowCount() == 0)
		return;

	// Add the section header here so we can clear it if necessary
	PopupMenuTitle *recentHeader = new PopupMenuTitle(i18n("Most Used Applications"), this);
	this->insertAction(m_allAppsHeader, recentHeader);
	m_recentActions.append(recentHeader);

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
KMenu::createApplicationsItems()
{
	// Create the search bar container
	QWidget *searchBar = new QWidget(this);
	int iconSize = style()->pixelMetric(QStyle::PM_SmallIconSize); // The size of menu item icons (by default)
	QStyleOptionMenuItem opt; // Call sizeFromContents to get the height of a menu item in the current QStyle,
	opt.initFrom(this);       // ensuring the search bar is the same height as a menu item, giving a more consistent look
	int menuHeight = style()->sizeFromContents(QStyle::CT_MenuItem, &opt, QSize(0, qMax(opt.fontMetrics.height(), iconSize))).height();
	searchBar->setFixedHeight(menuHeight);
	
	QHBoxLayout *layout = new QHBoxLayout(searchBar); // layout to hold the button and lineedit
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	
	QToolButton *clearBtn = new QToolButton(searchBar); // Clear button
	clearBtn->setFixedSize(menuHeight, menuHeight); // ensure the button is square	
	clearBtn->setIconSize(QSize(iconSize, iconSize));
	clearBtn->setIcon(QIcon::fromTheme(QStringLiteral("edit-clear")));
	clearBtn->setAutoRaise(true);
	clearBtn->setFocusPolicy(Qt::StrongFocus);
	
    m_searchLineEdit = new QLineEdit(searchBar); // Line edit
	m_searchLineEdit->setPlaceholderText(i18n("Press '/' to search..."));
	m_searchLineEdit->setFocusPolicy(Qt::StrongFocus);
	m_searchLineEdit->installEventFilter(this);

	layout->addWidget(clearBtn);
	layout->addWidget(m_searchLineEdit);

	// Create the action which contains our search bar
	QWidgetAction* searchAction = new QWidgetAction(this);
	searchAction->setDefaultWidget(searchBar);

	this->insertAction(m_actionsHeader, searchAction);

	// Create the '/' shortcut
	QShortcut *searchShortcut = new QShortcut(Qt::Key_Slash, this);

	// Connect the necessary signals to their respective slots
	connect(searchShortcut, &QShortcut::activated, m_searchLineEdit, qOverload<>(&QLineEdit::setFocus));
	connect(this, &QMenu::aboutToShow, m_searchLineEdit, &QLineEdit::clear);
	connect(clearBtn, &QToolButton::clicked, m_searchLineEdit, &QLineEdit::clear);
	connect(m_searchLineEdit, &QLineEdit::textChanged, this, &KMenu::updateSearchResults);	
	connect(KSycoca::self(), &KSycoca::databaseChanged, this, &KMenu::updateApplications);
	updateApplications(); // Call the function to populate the menu itself
}

void
KMenu::updateSearchResults()
{
	QLineEdit *lineEdit = qobject_cast<QLineEdit *>(sender());
	if (!lineEdit)
		return;

	QString text = lineEdit->text();

	std::function<bool(QAction *, QList<QAction *>)> setActionStates =
		[&](QAction *parent, QList<QAction *> children) {

			bool enableParent = false;
			
			for (QAction *action : children) {
				if (QMenu *subMenu = action->menu()) {
					// If the action is a sub menu, recurse through it and check to see if the current action (which
					// opens a sub menu) must be enabled, if so, set enableParent to true for now.
					enableParent = setActionStates(action, subMenu->actions());
				} else {					
					if (text.isEmpty() || (QStringView(action->data().toString()).left(text.length()).compare(text, Qt::CaseInsensitive) == 0)) {
						// Item must be enabled either if the search string matches the action name, or if the search bar is empty
						enableParent = true;
						action->setEnabled(true);
					} else
						action->setEnabled(false);
				}
			}

			if (parent)
				parent->setEnabled(enableParent);

			return enableParent;
		};

	setActionStates(nullptr, m_applicationActions);
}

void
KMenu::updateApplications()
{
	cleanupActionList(m_applicationActions);
	
	// The root of the applications menu
    KServiceGroup::Ptr root = KServiceGroup::root();

	if (!root || !root->isValid())
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
					action->setData(service->name()); // Store the unmodified name for searching
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
