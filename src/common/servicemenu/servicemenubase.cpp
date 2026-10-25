/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "servicemenu.h"
#include "servicegroupmenu.h"
#include "servicemenuapplet.h"
#include "popuptitle.h"
#include "containmentinterface.h"

#include <QMimeData>
#include <QMimeDatabase>
#include <QMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QStyle>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QAbstractItemModel>

#include <KLocalizedString>
#include <KNotificationJobUiDelegate>
#include <KApplicationTrader>
#include <KPropertiesDialog>
#include <KServiceGroup>

#include <KIO/ApplicationLauncherJob>
#include <KIO/CommandLauncherJob>
#include <KIO/OpenUrlJob>
#include <KIO/OpenFileManagerWindowJob>
#include <KIO/DesktopExecParser>

#include <PlasmaActivities/ResourceInstance>

ServiceMenuBase::ServiceMenuBase(ServiceMenuApplet *applet, QWidget *parent)
: ServiceMenuBase(QIcon(), QString(), applet, parent) {}

ServiceMenuBase::ServiceMenuBase(const QString &title, ServiceMenuApplet *applet, QWidget *parent)
: ServiceMenuBase(QIcon(), title, applet, parent) {}

ServiceMenuBase::ServiceMenuBase(const QIcon &icon, const QString &title, ServiceMenuApplet *applet, QWidget *parent)
	: QMenu(parent),
	  m_applet(applet)
{
	if (!icon.isNull())
		setIcon(icon);
	if (!title.isEmpty())
		setTitle(title);	
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &QMenu::customContextMenuRequested, this, &ServiceMenuBase::showContextMenu);
	loadConfig();
}

void
ServiceMenuBase::loadConfig()
{
	m_displayGenericName = applet() ? applet()->getConfigValue<bool>(QStringLiteral("showGenericName")) : true;
    setToolTipsVisible(applet() ? applet()->getConfigValue<bool>(QStringLiteral("showTooltips")) : false);
}

ServiceMenuApplet*
ServiceMenuBase::applet() const
{
	return m_applet;
}

void
ServiceMenuBase::actionEvent(QActionEvent *e)
{
	// If the action has an associated menu, delete the menu if it's
	// parented by this. NOTE: Removing an action *will* delete its
	// menu, so the menu would need to be recreated if the action
	// is re-added.
	if (e->type() == QEvent::ActionRemoved)
		if (QAction *action = e->action())
			if (QMenu *menu = action->menu())
				if (menu->parentWidget() == this)
					menu->deleteLater();

	QMenu::actionEvent(e);
}

QAction*
ServiceMenuBase::createActionFromService(const KService::Ptr &service, const QUrl &url)
{
	/* Takes a KService (and an optional url pointing to a file to open, and
	   creates a QAction which launches the service, parented to this. */
	if (!service || !service->isValid())
		return nullptr;

	QString title = escapeMnemonics(service->name());
	if (m_displayGenericName) {
		const QString genericName = escapeMnemonics(service->genericName());
		if (!genericName.isEmpty() && genericName != title)
			title.append(QStringLiteral(" (%1)").arg(genericName));
	}
	
	auto *action = new QAction(
		QIcon::fromTheme(service->icon(),
						 QIcon::fromTheme(QStringLiteral("application-x-executable"))),
		title, this);

	action->setData(QVariant::fromValue(service)); // Store the KService
	action->setToolTip(service->comment()); // Set the action tooltip
	
	connect(action, &QAction::triggered, this, [this, service, url]() {
		auto *job = new KIO::ApplicationLauncherJob(service);
		job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
		if (!url.isEmpty())
			job->setUrls({url});
		job->start();
		notifyAccessed(QUrl(QStringLiteral("applications:") + service->storageId()));
	});

	return action;
}

QAction*
ServiceMenuBase::createActionFromUrl(const QUrl &url)
{
	/* Returns a QAction* which launches a given QUrl, parented to this. */
	if (!url.isValid())
		return nullptr;
	
	const QString fileName = escapeMnemonics(url.fileName()); // name to display in the menu
		
	const QMimeDatabase db; // use QMimeDatabase to fetch the icon name
	const QMimeType mime = db.mimeTypeForFile(url.toLocalFile(), QMimeDatabase::MatchExtension);	
	const QIcon icon = QIcon::fromTheme(mime.iconName(), QIcon::fromTheme(QStringLiteral("text-x-generic")));

	auto *action = new QAction(icon, fileName, this);
	action->setData(url);
	action->setToolTip(url.toDisplayString(QUrl::PreferLocalFile));
	
	connect(action, &QAction::triggered, this, [this, url]() {
		auto *job = new KIO::OpenUrlJob(url);
		job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
		job->start();
		notifyAccessed(url);
	});

	return action;
}

QAction*
ServiceMenuBase::createFileExplorerActionFromUrl(const QUrl &url)
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
ServiceMenuBase::createActionFromKCM(const QString &id, const QString &name, const QString &icon)
{
	/* Returns a QAction* which launches a KCM with given id inside of
	   kcmshell6, with a given name and icon, parented to this. */

	if (id.isEmpty())
		return nullptr;

	auto *action = new QAction(QIcon::fromTheme(icon), escapeMnemonics(name), this);
	connect(action, &QAction::triggered, this, [id]() {
		auto *job = new KIO::CommandLauncherJob(QStringLiteral("kcmshell6"), {id});
		job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
		job->start();
	});

	return action;
}

QList<QAction *>
ServiceMenuBase::createActionsFromServiceActions(const KService::Ptr &service)
{
	/* Takes a KService and returns a list of QAction*s corresponding to 
	   each KServiceAction within the KService, all parented to this. */
	QList<QAction *> actions;	
	if (!service || !service->isValid())
		return actions;

	const QList<KServiceAction> serviceActions = service->actions();
	for (const KServiceAction &serviceAction : serviceActions) {
		if (serviceAction.isSeparator()) {
		    auto *action = new QAction(this);
			action->setSeparator(true);
			actions.append(action);
			continue;
		}
		
		const QString name = escapeMnemonics(serviceAction.text());
		if (name.isEmpty() || serviceAction.noDisplay() || serviceAction.exec().isEmpty())
			continue;

		auto *action = new QAction(QIcon::fromTheme(serviceAction.icon()), name, this);	  
		connect(action, &QAction::triggered, this, [this, service, serviceAction]() {
			auto *job = new KIO::ApplicationLauncherJob(serviceAction);
			job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
			job->start();
			notifyAccessed(QUrl(QStringLiteral("applications:") + service->storageId()));
		});

		actions.append(action);
	}

	return actions;
}

void
ServiceMenuBase::showContextMenu(const QPoint &pos) const
{
	if (!applet())
		return;
	ContainmentInterface *containmentInterface = applet()->containmentInterface();
	if (!containmentInterface)
		return; // should never happen, but just in case
	
	QAction *action = actionAt(pos);
	if (!action)
		return;

	ServiceMenuBase contextMenu(applet()); // No need for a parent since we're declaring on the stack
	contextMenu.setContextMenuPolicy(Qt::NoContextMenu); // disallow the contextmenu to spawn additional context menus

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
		QAction *runAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("system-run")), i18n("Put Into Run Dialog"));
		connect(runAction, &QAction::triggered, &contextMenu, [service](){invokeKRunner(KIO::DesktopExecParser::executablePath(service->exec()));});
		
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
				contextMenu.addAction(new PopupTitle(QIcon::fromTheme(QStringLiteral("document-open")),
													 i18n("Open With"), &contextMenu));

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
	if (!contextMenu.isEmpty())
		contextMenu.exec(mapToGlobal(pos));
}

void
ServiceMenuBase::runMenuEditor(QString arg)
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
ServiceMenuBase::invokeKRunner(const QString &arg)
{
	QDBusMessage query = QDBusMessage::createMethodCall(
		QStringLiteral("org.kde.krunner"),
		QStringLiteral("/App"),
		QStringLiteral("org.kde.krunner.App"),
		QStringLiteral("query"));
	query.setArguments({arg});

	QDBusMessage display = QDBusMessage::createMethodCall(
		QStringLiteral("org.kde.krunner"),
		QStringLiteral("/App"),
		QStringLiteral("org.kde.krunner.App"),
		QStringLiteral("display"));

	QDBusConnection bus = QDBusConnection::sessionBus();
	bus.send(query);
	bus.send(display);
}

QString
ServiceMenuBase::escapeMnemonics(QString string)
{
	return string.replace(QLatin1Char('&'), QStringLiteral("&&"));
}

void
ServiceMenuBase::notifyAccessed(const QUrl &url) const
{
	if (!url.isValid())
		return;
	KActivities::ResourceInstance::notifyAccessed(
		url,
		applet() ? applet()->pluginName() : QString()
		);	
}

/* Mouse events adapted from KDE 3.5 kicker source code.
   Copyright (c) 1996-2000 the KDE 3 kicker authors.
   Source available: https://kde.org/info/1-2-3/3.5.10/ */

void
ServiceMenuBase::mousePressEvent(QMouseEvent *ev)
{
	if (ev->button() == Qt::LeftButton)
		m_startPos = ev->position();
	
	// We need to separately handle when a QAction with a sub menu is right clicked
	// since by default Qt does not allow context menus for these items
	if (ev->button() == Qt::RightButton && contextMenuPolicy() != Qt::NoContextMenu) {
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
ServiceMenuBase::mouseMoveEvent(QMouseEvent *ev)
{
	QMenu::mouseMoveEvent(ev);
	if (!(ev->buttons() & Qt::LeftButton) || !m_startPos)		
		return;

	QPointF p = ev->position() - *m_startPos;
	if (p.manhattanLength() <= QApplication::startDragDistance() )
		return;

	QAction *action = actionAt(m_startPos->toPoint());
	if (!action || !action->isEnabled())
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
		const int iconSize = style()->pixelMetric(QStyle::PM_SmallIconSize);
		drag->setPixmap(action->icon().pixmap(QSize(iconSize, iconSize), devicePixelRatio()));
	}

	QPointer<ServiceMenuBase> guard(this); // guard in case the menu destroys itself
	drag->exec(Qt::CopyAction | Qt::LinkAction);
	if (!guard) return;
	m_startPos.reset();
}

void
ServiceMenuBase::mouseReleaseEvent(QMouseEvent *ev)
{
	m_startPos.reset();
	QMenu::mouseReleaseEvent(ev);	
}
