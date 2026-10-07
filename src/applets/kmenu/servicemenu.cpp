/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "servicemenu.h"
#include "kmenuapplet.h"
#include "popupmenutitle.h"
#include "containmentinterface.h"

#include <QMimeData>
#include <QMimeDatabase>
#include <QMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QStyle>
#include <QDBusInterface>

#include <KService>
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

ServiceMenu::ServiceMenu(KMenuApplet *applet, QWidget *parent)
: QMenu(parent),
  m_initialized(false),
  m_applet(applet)
{
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &QMenu::customContextMenuRequested, this, &ServiceMenu::showContextMenu, Qt::UniqueConnection);
}

ServiceMenu::ServiceMenu(const QString &title, KMenuApplet *applet, QWidget *parent)
	: QMenu(title, parent),
	  m_initialized(false),
	  m_applet(applet)
{
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &QMenu::customContextMenuRequested, this, &ServiceMenu::showContextMenu, Qt::UniqueConnection);
}

void
ServiceMenu::initialize()
{
	if (initialized()) return;
	m_displayGenericName = applet() ? applet()->getConfigValue<bool>(QStringLiteral("showGenericName")) : true;
	this->setToolTipsVisible(applet() ? applet()->getConfigValue<bool>(QStringLiteral("showTooltips")) : false);
	setInitialized(true);
}

void
ServiceMenu::reinitialize()
{
	if (!initialized())
		return;		
	setInitialized(false);

	clear();
	initialize();
}

KMenuApplet*
ServiceMenu::applet() const
{
	return m_applet;
}

void
ServiceMenu::actionEvent(QActionEvent *e)
{
	// If the action has an associated menu, delete the menu if it's
	// parented by this.
	if (e->type() == QEvent::ActionRemoved)
		if (QAction *action = e->action())
			if (QMenu *menu = action->menu())
				if (menu->parentWidget() == this)
					menu->deleteLater();

	QMenu::actionEvent(e);
}

void
ServiceMenu::cleanupActionList(QList<QAction *> &actionList)
{		
	// Cleans up all member actions of our QList from the menu
	for (QAction *action : actionList) {
		removeAction(action);
		action->deleteLater();
	}

	// Clear out the list itself
	actionList.clear();
}

QAction*
ServiceMenu::createActionFromService(const KService::Ptr &service, const QUrl &url)
{
	/* Takes a KService (and an optional url pointing to a file to open, and
	   creates a QAction which launches the service, parented to this. */
	if (!service || !service->isValid())
		return nullptr;

	QString title = service->name().replace(QLatin1Char('&'), QStringLiteral("&&"));
	if (m_displayGenericName) {
		const QString genericName = service->genericName().replace(QLatin1Char('&'), QStringLiteral("&&"));
		if (!genericName.isEmpty() && genericName != title)
			title.append(QStringLiteral(" (%1)").arg(genericName));
	}
	
	auto *action = new QAction(QIcon::fromTheme(service->icon()), title, this);

    action->setData(QVariant::fromValue(service)); // Store the KService
	action->setToolTip(service->comment()); // Set the action tooltip
	
	connect(action, &QAction::triggered, this, [service, url]() {
		auto *job = new KIO::ApplicationLauncherJob(service);
		job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
		if (!url.isEmpty())
			job->setUrls({url});
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
	/* Returns a QAction* which launches a given QUrl, parented to this. */
	if (!url.isValid())
		return nullptr;
	
    const QString fileName = url.fileName().replace(QLatin1Char('&'), QStringLiteral("&&")); // name to display in the menu
		
	const QMimeDatabase db; // use QMimeDatabase to fetch the icon name
    const QMimeType mime = db.mimeTypeForFile(url.toLocalFile(), QMimeDatabase::MatchExtension);
	const QIcon icon = QIcon::fromTheme(mime.iconName());

	auto *action = new QAction(icon, fileName, this);
	action->setData(url);
	
	connect(action, &QAction::triggered, this, [url]() {
		auto *job = new KIO::OpenUrlJob(url);
	    job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
		job->start();
		KActivities::ResourceInstance::notifyAccessed(
			url,
			QStringLiteral("com.github.neeeeow.klassik.kmenu")
			);			
	});

	return action;
}

QAction*
ServiceMenu::createFileExplorerActionFromUrl(const QUrl &url)
{
	/* Returns a QAction* which opens the file pointed to by the QUrl
	   in the file explorer, parented to this. */
	if (!url.isValid())
		return nullptr;
	
    auto *action = new QAction(QIcon::fromTheme(QStringLiteral("system-file-manager")), i18n("Open in File Explorer"), this);
	connect(action, &QAction::triggered, this, [url]() {
		auto *job = new KIO::OpenFileManagerWindowJob();
		job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
		job->setHighlightUrls({url});
		job->start();
	});

	return action;
}

QAction*
ServiceMenu::createActionFromKCM(const QString &id, const QString &name, const QString &icon)
{
	/* Returns a QAction* which launches a KCM with given id inside of
	   kcmshell6, with a given name and icon, parented to this. */

	if (id.isEmpty())
		return nullptr;

	QString actionText = name;
	auto *action = new QAction(QIcon::fromTheme(icon), actionText.replace(QLatin1Char('&'), QStringLiteral("&&")), this);
	connect(action, &QAction::triggered, this, [id]() {
		auto *job = new KIO::CommandLauncherJob(QStringLiteral("kcmshell6"), {id});
		job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
		job->start();
	});

	return action;
}

QList<QAction *>
ServiceMenu::createActionsFromServiceActions(const KService::Ptr &service)
{
	/* Takes a KService and returns a list of QAction*s corresponding to 
	   each KServiceAction within the KService, all parented to this. */
	QList<QAction *> actions;	
	if (!service || !service->isValid())
		return actions;

	const QList<KServiceAction> serviceActions = service->actions();
	for (const KServiceAction &serviceAction : serviceActions) {
		const QString name = serviceAction.text().replace(QLatin1Char('&'), QStringLiteral("&&"));
		const QString exec = serviceAction.exec();
		if (name.isEmpty() || exec.isEmpty()) {
			continue;
		}

		auto *action = new QAction(QIcon::fromTheme(serviceAction.icon()), name, this);   
		connect(action, &QAction::triggered, this, [service, exec]() {
			auto *job = new KIO::CommandLauncherJob(exec);
			job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
			job->start();				
			KActivities::ResourceInstance::notifyAccessed(
				QUrl(QStringLiteral("applications:") + service->storageId()),
				QStringLiteral("com.github.neeeeow.klassik.kmenu")
				);			
		});

		actions.append(action);
	}

	return actions;
}

QList<QAction *>
ServiceMenu::createActionsFromServiceGroup(const KServiceGroup::Ptr &group)
{
	/* Takes a KServiceGroup, and returns a QList of actions corresponding to every
	   KService in the group, all in the correct hierarchy. */
	QList<QAction *> actions;
	if (!group || !group->isValid())
		return actions;

	const auto entries = group->entries(true);
	for (const auto &entry : entries) {
		if (entry->isType(KST_KService)) {
			// If the entry is a service, it's an individual application
			const KService::Ptr service(static_cast<KService*>(entry.data()));
			if (QAction *action = createActionFromService(service))
				actions.append(action);
		} else if (entry->isType(KST_KServiceGroup)) {
			// If the entry is a service group, we need to make a submenu and recurse through this function
			const KServiceGroup::Ptr subGroup(static_cast<KServiceGroup*>(entry.data()));
			if (subGroup->childCount() == 0)
				continue;
					
			auto *subMenu = new ServiceMenu(subGroup->caption().replace(QStringLiteral("&"), QStringLiteral("&&")), applet(), this);
			subMenu->initialize();
			subMenu->setIcon(QIcon::fromTheme(subGroup->icon()));
			subMenu->addActions(subMenu->createActionsFromServiceGroup(subGroup));

			auto *action = new QAction(subMenu->icon(), subMenu->title(), this);
			action->setMenu(subMenu);
			action->setData(QVariant::fromValue(subGroup));
			actions.append(action);	
		}
	}
	return actions;
}

void
ServiceMenu::showContextMenu(const QPoint &pos) const
{
	if (!applet())
		return;
	ContainmentInterface *containmentInterface = applet()->containmentInterface();
	if (!containmentInterface)
		return; // should never happen, but just in case
	
	QAction *action = actionAt(pos);
	if (!action)
		return;

	ServiceMenu contextMenu(applet()); // No need for a parent since we're declaring on the stack
	contextMenu.setContextMenuPolicy(Qt::NoContextMenu); // disallow the contextmenu to spawn additional context menus
	contextMenu.initialize();

	if (action->data().canConvert<KService::Ptr>()) {
		// KService means application!
		const KService::Ptr service = action->data().value<KService::Ptr>();

		// Actions corresponding to the KServiceActions
		const QList<QAction *> serviceSubActions = contextMenu.createActionsFromServiceActions(service);
		if (!serviceSubActions.isEmpty())
			contextMenu.addActions(serviceSubActions);

		if (!contextMenu.isEmpty())
			contextMenu.addSeparator();

		// Actions for adding the item to the desktop/quicklaunch/panel
		if (containmentInterface->mayAddLauncher(ContainmentInterface::Desktop)) {
			QAction *desktopAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add to Desktop"));
			connect(desktopAction, &QAction::triggered, containmentInterface, [containmentInterface, service]() {
				containmentInterface->addLauncher(ContainmentInterface::Desktop, service);
			});
		}
		if (containmentInterface->mayAddLauncher(ContainmentInterface::Quicklaunch)) {
			QAction *launcherAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add to Quicklaunch"));
			connect(launcherAction, &QAction::triggered, containmentInterface, [containmentInterface, service]() {
				containmentInterface->addLauncher(ContainmentInterface::Quicklaunch, service);
			});
		}
		if (containmentInterface->mayAddLauncher(ContainmentInterface::Panel)) {
			QAction *panelAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add to Panel"));
			connect(panelAction, &QAction::triggered, containmentInterface, [containmentInterface, service]() {
				containmentInterface->addLauncher(ContainmentInterface::Panel, service);
			});
		}

		if (!contextMenu.isEmpty())
			contextMenu.addSeparator();

		// Action for editing the menu item
		QAction *editAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("kmenuedit")), i18n("Edit Application"));
		connect(editAction, &QAction::triggered, &contextMenu, [service]() {runMenuEditor(service->menuId());});

		// Action to put the action's exec command in krunner
		QAction *runAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("run")), i18n("Put Into Run Dialog"));
	    connect(runAction, &QAction::triggered, &contextMenu, [service](){invokeKRunner(service->exec());});
		
	} else if (action->data().canConvert<KServiceGroup::Ptr>()) {
		// KServiceGroup means sub menu container
		const KServiceGroup::Ptr serviceGroup = action->data().value<KServiceGroup::Ptr>();
		QAction *editAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("kmenuedit")), i18n("Edit Menu"));
		connect(editAction, &QAction::triggered, &contextMenu, [serviceGroup]() {
			runMenuEditor(serviceGroup->relPath());
		});
	} else if (action->data().canConvert<QUrl>()) {
		// QUrl means a file path
		const QUrl url = action->data().toUrl();
		const KFileItem fileItem(url);

		const KService::List services = KApplicationTrader::queryByMimeType(fileItem.mimetype());

		if (!services.isEmpty()) {
			if (applet()->getConfigValue<bool>(QStringLiteral("showTitles")))
				contextMenu.addAction(new PopupMenuTitle(i18n("Open With"), &contextMenu));

			for (const KService::Ptr &service : services) {
				if (QAction *serviceAction = contextMenu.createActionFromService(service, url))
					contextMenu.addAction(serviceAction);
			}						
		}

		contextMenu.addSeparator();
		QAction *propertiesAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("document-properties")), i18n("Properties"));
		connect(propertiesAction, &QAction::triggered, &contextMenu, [url]() {
			auto *dlg = new KPropertiesDialog(url, QApplication::activeWindow());
			dlg->setAttribute(Qt::WA_DeleteOnClose);
			dlg->show();
		});
		
		QAction *fileExplorerAction = contextMenu.createFileExplorerActionFromUrl(url);
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
		QAction *action = actionAt(ev->position().toPoint());
		if (action && action->menu()) {
			action->menu()->close();
			showContextMenu(ev->position().toPoint());
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

	QPointer<ServiceMenu> guard(this); // guard in case the menu destroys itself
	drag->exec(Qt::CopyAction | Qt::LinkAction);
	if (!guard) return;
	m_startPos = QPointF(-1.0, -1.0);
}
