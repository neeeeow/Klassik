#include "servicemenu.h"
#include "popupmenutitle.h"
#include "containmentinterface.h"

#include <QList>
#include <QMimeData>
#include <QMimeDatabase>
#include <QMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QStyle>
#include <QDBusInterface>

#include <KLocalizedString>
#include <KNotificationJobUiDelegate>
#include <KFileItem>
#include <KApplicationTrader>
#include <KPropertiesDialog>

#include <KIO/ApplicationLauncherJob>
#include <KIO/CommandLauncherJob>
#include <KIO/OpenUrlJob>
#include <KIO/OpenFileManagerWindowJob>

#include <PlasmaActivities/ResourceInstance>

#include <Plasma/Applet>

ServiceMenu::ServiceMenu(KMenuApplet *applet, QWidget *parent)
: QMenu(parent),
  m_initialized(false),
  m_applet(applet)
{
}

ServiceMenu::ServiceMenu(const QString &title, KMenuApplet *applet, QWidget *parent)
	: QMenu(title, parent),
	  m_initialized(false),
	  m_applet(applet)
{
}

void
ServiceMenu::initialize()
{
	if (initialized()) return;	
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &QMenu::customContextMenuRequested, this, &ServiceMenu::showContextMenu, Qt::UniqueConnection);	
	setInitialized(true);
}

void
ServiceMenu::reinitialize()
{
	if (!initialized())
		return;
		
	setInitialized(false);

	// Clear out the menu
	QList<QAction *> allActions = actions();
	cleanupActionList(allActions);
	clear();

	// Finally, call initialize() again
	initialize();
}

bool
ServiceMenu::initialized()
{
	return m_initialized;
}

void
ServiceMenu::setInitialized(bool initialized)
{
	m_initialized = initialized;
}

void
ServiceMenu::cleanupActionList(QList<QAction *> &actionList)
{		
	// Cleans up all member actions of our QList from the menu
	for (QAction *action : actionList) {
		removeAction(action);
		if (action) {
			if (QMenu *menu = action->menu())
				menu->deleteLater(); // submenus must be cleared here too
			action->deleteLater();			
		}
	}

	// Clear out the list itself
	actionList.clear();
}

QAction*
ServiceMenu::createActionFromService(const KService::Ptr &service, const QUrl &url)
{
	if (!service || !service->isValid())
		return nullptr;
	
	QAction *action = new QAction(QIcon::fromTheme(service->icon()), service->name().replace(QStringLiteral("&"), QStringLiteral("&&")), this);

    action->setData(QVariant::fromValue(service)); // Store the KService
	
	connect(action, &QAction::triggered, this, [service, url]() {
		auto *job = new KIO::ApplicationLauncherJob(service);
		job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
		if (!url.isEmpty())
			job->setUrls({url});
		job->setAutoDelete(true);
		job->start();
		KActivities::ResourceInstance::notifyAccessed(
			QUrl(QStringLiteral("applications:") + service->storageId()),
			QStringLiteral("com.github.neeeeow.klassik.kmenu")
			);			
	});

	return action;
}

QAction*
ServiceMenu::createActionFromUrl(const QUrl &url)
{
	if (!url.isValid())
		return nullptr;
	
	QString fileName = url.fileName().replace(QStringLiteral("&"), QStringLiteral("&&")); // name to display in the menu
		
	QMimeDatabase db; // use QMimeDatabase to fetch the icon name
	QMimeType mime = db.mimeTypeForUrl(url);
	QIcon icon = QIcon::fromTheme(mime.iconName());

	QAction *action = new QAction(icon, fileName, this);
	action->setData(url);
	
	connect(action, &QAction::triggered, this, [url]() {
		auto *job = new KIO::OpenUrlJob(url);
	    job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
		job->setAutoDelete(true);
		job->start();
		KActivities::ResourceInstance::notifyAccessed(
			url,
			QStringLiteral("com.github.neeeeow.klassik.kmenu")
			);			
	});

	return action;
}

QAction*
ServiceMenu::createFileExplorerActionFromUrl(const QUrl &url, const QIcon &icon, const QString &title)
{
	if (!url.isValid())
		return nullptr;
	
    QAction *action = new QAction(icon, title, this);

	connect(action, &QAction::triggered, this, [url]() {
		auto *job = new KIO::OpenFileManagerWindowJob();
		job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
		job->setHighlightUrls({url});
		job->setAutoDelete(true);
		job->start();
	});

	return action;
}

QList<QAction*>
ServiceMenu::actionListFromServiceGroup(const KServiceGroup::Ptr &root)
{
	// TODO: improve this function
	QList<QAction *> actionList;

	if (!root || !root->isValid())
		return actionList;

	// Define a recursive lambda for traversing the service groups and populating the submenus
	auto populateSubmenu = [&](this auto&& self, ServiceMenu *parent, KServiceGroup::Ptr group) -> void {
		for (const auto &entry : group->entries(true)) {
			if (entry->isType(KST_KService)) {
				// If the entry is a service, it's an individual application
				KService::Ptr service(static_cast<KService*>(entry.data()));

				// Create the entry
				QAction *action = parent->createActionFromService(service, QUrl());
				if (!action)
					continue;

				if (parent == this) { // Don't add top-level actions to the menu, add them to our QList.
					actionList.append(action);
				} else
					parent->addAction(action);
			} else if (entry->isType(KST_KServiceGroup)) {
				// If the entry is a service group, we need to make a submenu and recurse through this function
				KServiceGroup::Ptr subGroup(static_cast<KServiceGroup*>(entry.data()));
				if (subGroup->childCount() == 0)
					continue;
					
				ServiceMenu *subMenu = new ServiceMenu(subGroup->caption().replace(QStringLiteral("&"), QStringLiteral("&&")), applet(), parent);
				subMenu->initialize();
				subMenu->setIcon(QIcon::fromTheme(subGroup->icon()));

				if (parent == this) {
					QAction *action = new QAction(subMenu->icon(), subMenu->title(), parent);
					action->setMenu(subMenu);
					action->setData(QVariant::fromValue(subGroup));
				    actionList.append(action);						
				} else {
					QAction *action = parent->addMenu(subMenu);
					action->setData(QVariant::fromValue(subGroup));
				}

				self(subMenu, subGroup);
			}
		}
			
	};

	populateSubmenu(this, root);
	
	return actionList;
}

void
ServiceMenu::showContextMenu(const QPoint &pos)
{
	ContainmentInterface *containmentInterface = applet()->containmentInterface();
	if (!containmentInterface)
		return; // should never happen, but just in case
	
	QAction *action = actionAt(pos);
	if (!action)
		return;

	ServiceMenu contextMenu(applet(), this);

	if (action->data().canConvert<KService::Ptr>()) {
		// KService means application!
		KService::Ptr service = action->data().value<KService::Ptr>();
		
		if (containmentInterface->mayAddLauncher(ContainmentInterface::Desktop)) {
			QAction *desktopAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add to Desktop"));
			connect(desktopAction, &QAction::triggered, containmentInterface, [containmentInterface, service]() {
				containmentInterface->addLauncher(ContainmentInterface::Desktop, service);
			});
		}

		if (containmentInterface->mayAddLauncher(ContainmentInterface::TaskManager)) {
			if (!containmentInterface->hasLauncher(ContainmentInterface::TaskManager, service)) {
				QAction *taskManagerAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("pin")), i18n("Pin to Task Manager"));
				connect(taskManagerAction, &QAction::triggered, containmentInterface, [containmentInterface, service]() {
					containmentInterface->addLauncher(ContainmentInterface::TaskManager, service);
				});
			}
		}

		if (containmentInterface->mayAddLauncher(ContainmentInterface::Panel)) {
			QAction *panelAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add to Panel"));
			connect(panelAction, &QAction::triggered, containmentInterface, [containmentInterface, service]() {
				containmentInterface->addLauncher(ContainmentInterface::Panel, service);
			});
		}

		if (!contextMenu.isEmpty())
			contextMenu.addSeparator();

		QAction *editAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("kmenuedit")), i18n("Edit Application"));
		connect(editAction, &QAction::triggered, &contextMenu, [this, service]() {
			runMenuEditor(service->menuId());
		});

		QAction *runAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("run")), i18n("Put Into Run Dialog"));
	    connect(runAction, &QAction::triggered, this, [service](){invokeKRunner(service->exec());});
		
	} else if (action->data().canConvert<KServiceGroup::Ptr>()) {
		// KServiceGroup means sub menu container
		KServiceGroup::Ptr serviceGroup = action->data().value<KServiceGroup::Ptr>();
		QAction *editAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("kmenuedit")), i18n("Edit Menu"));
		connect(editAction, &QAction::triggered, &contextMenu, [this, serviceGroup]() {
			runMenuEditor(serviceGroup->relPath());
		});
	} else if (action->data().canConvert<QUrl>()) {
		// QUrl means a file path
		QUrl url = action->data().toUrl();
		KFileItem fileItem(url);

		const KService::List services = KApplicationTrader::queryByMimeType(fileItem.mimetype());

		if (!services.isEmpty()) {
			if (applet()->getConfigValue<bool>(QStringLiteral("showTitles")))
				contextMenu.addAction(new PopupMenuTitle(i18n("Open With"), &contextMenu));

			for (const KService::Ptr &service : services) {
				QAction *action = contextMenu.createActionFromService(service, url);
				if (!action)
					continue;
				contextMenu.addAction(action);
			}						
		}

		contextMenu.addSeparator();
		QAction *propertiesAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("document-properties")), i18n("Properties"));
		connect(propertiesAction, &QAction::triggered, &contextMenu, [url]() {
			auto *dlg = new KPropertiesDialog(url, QApplication::activeWindow());
			dlg->setAttribute(Qt::WA_DeleteOnClose);
			dlg->show();
		});
		
		QAction *fileExplorerAction = contextMenu.createFileExplorerActionFromUrl(url, QIcon::fromTheme(QStringLiteral("system-file-manager")), i18n("Open in File Explorer"));
		if (fileExplorerAction) {			
			contextMenu.addSeparator();
			contextMenu.addAction(fileExplorerAction);
		}
		
	} else
		return;

	// Display the menu
	contextMenu.exec(mapToGlobal(pos));
}

void
ServiceMenu::runMenuEditor(QString arg)
{
	const auto service = KService::serviceByDesktopName(QStringLiteral("org.kde.kmenuedit"));
	if (!service) {
		qWarning() << "Could not find kmenuedit";
		return;
	}
	
	if (arg.isEmpty()) {
		arg = QStringLiteral("/"); // If already open, will collapse editor tree
	}

    auto *job = new KIO::CommandLauncherJob(service->exec(), {arg});
	job->setDesktopName(service->desktopEntryName());
	job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoErrorHandlingEnabled));
	job->start();
}

void
ServiceMenu::invokeKRunner(QString arg)
{
	QDBusInterface krunner(QStringLiteral("org.kde.krunner"), QStringLiteral("/App"), QStringLiteral("org.kde.krunner.App"));
	if (!krunner.isValid())
		return;

	if (arg.isEmpty())
		arg = QStringLiteral("");

	krunner.call(QStringLiteral("query"), arg);

	krunner.call(QStringLiteral("display"));
}

/* Mouse events adapted from KDE 3.5 kicker source code.
   Copyright (c) 1996-2000 the KDE 3 kicker authors.
   Source available: https://kde.org/info/1-2-3/3.5.10/ */

void
ServiceMenu::mousePressEvent(QMouseEvent *ev)
{
	if (ev->button() == Qt::LeftButton)
		m_startPos = ev->position();

	// If the right-clicked action has a submenu, we must
	// close it before displaying the context menu
	if (ev->button() == Qt::RightButton) {
		QAction *action = actionAt(ev->pos());
		if (action && action->menu()) {
			action->menu()->close();
			showContextMenu(ev->pos());
			return;
		}		
	}
	
	QMenu::mousePressEvent(ev);
}

void
ServiceMenu::mouseMoveEvent(QMouseEvent *ev)
{
	if (!(ev->buttons() & Qt::LeftButton)) {
		QMenu::mouseMoveEvent(ev);
		return;
    }

	if (m_startPos == QPointF(-1.0, -1.0))
		return;

	QPointF p = ev->position() - m_startPos;
	if (p.manhattanLength() <= QApplication::startDragDistance() )
        return;

	QAction *action = actionAt(m_startPos.toPoint());
	if (!action)
		return;

	QUrl url = action->data().toUrl();
	if (!url.isValid()) {
		if (action->data().canConvert<KService::Ptr>()) {
			KService::Ptr service = action->data().value<KService::Ptr>();
			url = QUrl::fromLocalFile(service->entryPath());
		} else
			return;
	}

	QDrag *drag = new QDrag(this);
	QMimeData *mimeData = new QMimeData;

	mimeData->setUrls({url});
	drag->setMimeData(mimeData);

	if (!action->icon().isNull()) {
		int iconSize = style()->pixelMetric(QStyle::PM_SmallIconSize);
		drag->setPixmap(action->icon().pixmap(iconSize, iconSize));
	}

	drag->exec(Qt::CopyAction | Qt::LinkAction);

	m_startPos = QPointF(-1.0, -1.0);
}
