/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "containmentinterface.h"
#include "servicemenuapplet.h"
#include "servicemenu.h"
#include "popuptitle.h"

#include <QQuickItem>
#include <QPoint>
#include <QRect>
#include <QQuickWindow>
#include <QTimer>
#include <QDBusConnection>
#include <QDBusMessage>

#include <KConfigPropertyMap>
#include <KConfigLoader>
#include <KLocalizedString>
#include <KNotificationJobUiDelegate>
#include <KProtocolManager>
#include <KFileItem>
#include <KApplicationTrader>
#include <KPropertiesDialog>

#include <KIO/ApplicationLauncherJob>
#include <KIO/CommandLauncherJob>
#include <KIO/OpenUrlJob>
#include <KIO/OpenFileManagerWindowJob>
#include <KIO/DesktopExecParser>

#include <PlasmaActivities/ResourceInstance>
#include <PlasmaQuick/AppletQuickItem>

ServiceMenuApplet::ServiceMenuApplet(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args)
	: Plasma::Applet(parentObject, data, args) {}

ServiceMenuApplet::~ServiceMenuApplet()
{
	deleteMenu();
}

void
ServiceMenuApplet::init()
{
	// init() should only ever be called once. If these pointers are not null, then something
	// has gone wrong.
	Q_ASSERT(!m_menu);
	Q_ASSERT(!m_containmentInterface);
	Q_ASSERT(!m_timer);
	
	// Call the original init function
	Plasma::Applet::init();

	// Set up the QTimer
	m_timer = new QTimer(this);
	m_timer->setSingleShot(true);
	m_timer->setInterval(0);
	if (configuration())
		connect(configuration(), &KConfigPropertyMap::valueChanged, m_timer, qOverload<>(&QTimer::start));		
	
	// Create the objects
	m_containmentInterface = new ContainmentInterface(this);
	initMenu();	
}

ContainmentInterface*
ServiceMenuApplet::containmentInterface() const
{
	return m_containmentInterface;
}

QVariant
ServiceMenuApplet::configValue(const QString &key) const
{
	KConfigLoader *configLoader = configScheme();
	if (!configLoader)
		return QVariant();	
	
	const QVariant value = configLoader->property(key);
	if (!value.isValid()) {
		qWarning() << "key" << key << "not found!";
		return QVariant();
	}
	return value;
}

void
ServiceMenuApplet::initMenu()
{
	// Delete any old menu objects
	deleteMenu();

	// Create the menu object. If it fails to create, simply bail out.
	m_menu = createMenu();
	if (m_menu) {
		connect(m_menu, &QMenu::aboutToShow, this, &ServiceMenuApplet::menuActiveChanged, Qt::QueuedConnection);
		connect(m_menu, &QMenu::aboutToHide, this, &ServiceMenuApplet::menuActiveChanged, Qt::QueuedConnection);
		if (m_timer)
			connect(m_timer, &QTimer::timeout, m_menu, &ServiceMenu::reinitialize);
	}

	// Emit the required signals
	Q_EMIT hasMenuChanged();
	Q_EMIT menuActiveChanged();

}

void
ServiceMenuApplet::deleteMenu()
{
	if (!m_menu)
		return;

	m_menu->disconnect(this);
	m_menu->close();
	m_menu->deleteLater();
	m_menu.clear();
}

bool
ServiceMenuApplet::menuActive() const
{
	return m_menu && m_menu->isVisible();
}

void
ServiceMenuApplet::toggleMenu()
{
	auto *item = PlasmaQuick::AppletQuickItem::itemForApplet(this);
	if (!m_menu || !item)
		return;

	if (m_menu->isVisible())
		m_menu->close();
	else {
		m_menu->createWinId();
		if (QWindow *menuWindow = m_menu->windowHandle())
			menuWindow->setTransientParent(item->window());
		m_menu->refreshContents();
		m_menu->popup(popupPosition());
	}
}

QPoint
ServiceMenuApplet::popupPosition()
{
	auto *item = PlasmaQuick::AppletQuickItem::itemForApplet(this);
	if (!m_menu || !item || !item->window() || !item->window()->screen())
		return QPoint();

	const Plasma::Types::Location panelLocation = location();

	m_menu->ensurePolished();
	const QSize menuSize = m_menu->sizeHint();
	QPoint pos = item->mapToGlobal(QPointF(0, 0)).toPoint();
	QRect parentGeometryBounds(pos, item->size().toSize());

	const QPoint topPoint(pos.x(), parentGeometryBounds.top() - menuSize.height());
	const QPoint bottomPoint(pos.x(), parentGeometryBounds.bottom());
	const QPoint leftPoint(parentGeometryBounds.left() - menuSize.width(), pos.y());
	const QPoint rightPoint(parentGeometryBounds.right(), pos.y());

	QPoint dialogPos;
	switch (panelLocation) {
	case Plasma::Types::TopEdge:
		dialogPos = bottomPoint;
		break;
	case Plasma::Types::LeftEdge:
		dialogPos = rightPoint;
		break;
	case Plasma::Types::RightEdge:
		dialogPos = leftPoint;
		break;
	default:
		dialogPos = topPoint;
		break;
	}

	QRect avail = item->window()->screen()->availableGeometry();

	// If popup goes out of bounds...
	// ...at the left edge
	if (dialogPos.x() < avail.left()) {
		if (panelLocation != Plasma::Types::LeftEdge) {
			// move it in bounds
			// Note: floating popup goes here.
			dialogPos.setX(avail.left());
		} else {
			// flip it around
			dialogPos.setX(rightPoint.x());
		}
	}
	// ...at the right edge
	if (dialogPos.x() + menuSize.width() > avail.right()) {
		if (panelLocation != Plasma::Types::RightEdge) {
			// move it in bounds
			// Note: floating popup goes here.
			dialogPos.setX(qMax(avail.left(), (avail.right() - menuSize.width() + 1)));
		} else {
			// flip it around
			dialogPos.setX(leftPoint.x());
		}
	}
	// ...at the top edge
	if (dialogPos.y() < avail.top()) {
		if (panelLocation == Plasma::Types::LeftEdge || panelLocation == Plasma::Types::RightEdge) {
			// move it in bounds
			dialogPos.setY(avail.top());
		} else {
			// flip it around
			// Note: floating popup goes here.
			dialogPos.setY(bottomPoint.y());
		}
	}
	// ...at the bottom edge
	if (dialogPos.y() + menuSize.height() > avail.bottom()) {
		if (panelLocation == Plasma::Types::LeftEdge || panelLocation == Plasma::Types::RightEdge) {
			// move it in bounds
			dialogPos.setY(qMax(avail.top(), (avail.bottom() - menuSize.height() + 1)));
		} else {
			// flip it around
			// Note: floating popup goes here.
			dialogPos.setY(topPoint.y());
		}
	}
	
	return dialogPos;
}

KService::Ptr
ServiceMenuApplet::serviceFromUrl(const QUrl &url)
{
	if (!url.isValid() || !url.isLocalFile())
		return KService::Ptr();

	const KService::Ptr service = KService::serviceByDesktopPath(url.toLocalFile());
	if (!service || !service->isValid())
		return KService::Ptr();
	
	return service;
}

QUrl
ServiceMenuApplet::urlFromServiceGroup(const KServiceGroup::Ptr &group)
{
	if (!group || !group->isValid())
		return QUrl();

	QUrl url;
	url.setScheme(QLatin1String("applications"));
	url.setPath(QLatin1Char('/') + group->relPath());

	return url;
}

KServiceGroup::Ptr
ServiceMenuApplet::serviceGroupFromUrl(const QUrl &url)
{
	if (!url.isValid() || url.scheme() != QLatin1String("applications")
		|| !url.path().startsWith(QLatin1Char('/')))
		return KServiceGroup::Ptr();

	const KServiceGroup::Ptr group = KServiceGroup::group(url.path().mid(1));
	if (!group || !group->isValid())
		return KServiceGroup::Ptr();
	
	return group;
}

void
ServiceMenuApplet::execService(const KService::Ptr &service, const QUrl &url)
{
	if (!service || !service->isValid())
		return;
	auto *job = new KIO::ApplicationLauncherJob(service);
	job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
	if (!url.isEmpty())
		job->setUrls({url});
	job->start();

	if (!url.isEmpty())
		// If a QUrl was specified, we notify that the url was accessed, not the application
		// launching the url
		KActivities::ResourceInstance::notifyAccessed(url);
	else
		KActivities::ResourceInstance::notifyAccessed(QUrl(QStringLiteral("applications:") + service->storageId()));
}

void
ServiceMenuApplet::execUrl(const QUrl &url)
{
	if (!url.isValid())
		return;
	auto *job = new KIO::OpenUrlJob(url);
	job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
	job->start();
	KActivities::ResourceInstance::notifyAccessed(url);
}

QList<QAction *>
ServiceMenuApplet::contextualActions()
{
	// First clear out the old set of actions
	for (auto action : m_actions)
		action->deleteLater();
	m_actions.clear();

	m_actions = contextualActions(appletUrl(), this);

	// Append a separator to separate out contextual actions from the internal actions
	if (!m_actions.isEmpty())
		appendSeparatorActionToList(m_actions, this);
	
	return m_actions + Applet::contextualActions();
}

QList<QAction *>
ServiceMenuApplet::contextualActions(const QUrl &url, QObject *parent)
{
	QList<QAction *> actions;
	if (!url.isValid())
		return actions;

	// Get pointers to a potential KService or KServiceGroup
	const KService::Ptr service = serviceFromUrl(url);
	const KServiceGroup::Ptr group = serviceGroupFromUrl(url);

	if (service)
		// First add the jump list actions for services
	    actions.append(jumpListActions(service, parent));
	else if (!group)
		// And add the open with actions for any QUrl which isn't a service, service group
	    actions.append(openWithActions(url, parent));

	if (!actions.isEmpty())
	    appendSeparatorActionToList(actions, parent);

	// Now add the open in file explorer action, only for normal QUrls
	if (!service && !group) {
		QAction *fileExplorerAction = openInFileExplorerAction(url, parent);
		if (fileExplorerAction) {						
		    actions.append(fileExplorerAction);
		    appendSeparatorActionToList(actions, parent);
		}
	}

	// Configuration section
	if (service || group) {
		// If we have a service or service group, we want to add the option to open the menu editor
		if (QAction *editAction = menuEditorAction(url, parent))
			actions.append(editAction);

		if (service) {
			// Furthermore for services, we add the option to put it in a KRunner dialog.
			if (QAction *runAction = kRunnerAction(KIO::DesktopExecParser::executablePath(service->exec()), parent))
				actions.append(runAction);
		}
	} else {
		// In any other case, place a standard properties action
		auto *propertiesAction = new QAction(QIcon::fromTheme(QStringLiteral("document-properties")), i18n("Properties"), parent);
		connect(propertiesAction, &QAction::triggered, propertiesAction, [url]() {
			auto *dlg = new KPropertiesDialog(url);
			dlg->setAttribute(Qt::WA_DeleteOnClose);
			dlg->setFileNameReadOnly(true);
			dlg->setWindowIcon(QIcon::fromTheme(QStringLiteral("document-properties")));
			dlg->show();
		});
		actions.append(propertiesAction);
	}	

	return actions;
}

QList<QAction *>
ServiceMenuApplet::jumpListActions(const KService::Ptr &service, QObject *parent)
{
	QList<QAction *> actions;	
	if (!service || !service->isValid())
		return actions;

	actions.append(new PopupTitle(QIcon::fromTheme(service->icon()), service->name(), parent));
	// We always add the option to launch the application
    auto *runAction = new QAction(QIcon::fromTheme(QStringLiteral("system-run")),
								  i18n("Run Application…"), parent);
	connect(runAction, &QAction::triggered, runAction, [service]() {
		execService(service);
	});
	actions.append(runAction);
	
	// Add the actual jumplist actions
	const QList<KServiceAction> serviceActions = service->actions();
	for (const KServiceAction &serviceAction : serviceActions) {
		if (serviceAction.noDisplay())
			continue;

		auto *action = new QAction(QIcon::fromTheme(serviceAction.icon()),
								   escapeMnemonics(serviceAction.text()), parent);
		if (serviceAction.isSeparator())
			action->setSeparator(true);

		connect(action, &QAction::triggered, action, [service, serviceAction]() {
			auto *job = new KIO::ApplicationLauncherJob(serviceAction);
			job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
			job->start();
			KActivities::ResourceInstance::notifyAccessed(QUrl(QStringLiteral("applications:") + service->storageId()));
		});

		actions.append(action);
	}

	return actions;
}

QList<QAction *>
ServiceMenuApplet::openWithActions(const QUrl &url, QObject *parent)
{
	QList<QAction *> actions;
	if (!url.isValid() || url.scheme().isEmpty())
		return actions;

	const KFileItem fileItem(url);
	if (fileItem.isDir())
		// We don't want to provide the options to open directories
		// with a certain application.
		return actions;

	const KService::List services = KApplicationTrader::queryByMimeType(fileItem.mimetype());
	for (const KService::Ptr &service : services) {
		if (!service || !service->isValid() || service->noDisplay())
			continue;

		auto *action = new QAction(
			QIcon::fromTheme(service->icon(),
							 QIcon::fromTheme(QStringLiteral("application-x-executable"))),
			escapeMnemonics(service->name()), parent);

		connect(action, &QAction::triggered, action, [service, url]() {
			execService(service, url);
		});
		actions.append(action);
	}
	
	if (!actions.isEmpty())
		actions.prepend(new PopupTitle(QIcon::fromTheme(QStringLiteral("document-open")),
									 i18n("Open With"), parent));

	return actions;
}

QAction*
ServiceMenuApplet::openInFileExplorerAction(const QUrl &url, QObject *parent)
{
	if (!url.isValid() || url.scheme().isEmpty() ||
		!KProtocolManager::supportsListing(url))
		return nullptr;
	
	auto *action = new QAction(QIcon::fromTheme(QStringLiteral("system-file-manager")),
							   i18n("Open in File Explorer"), parent);
	if (KFileItem(url).isDir()) {
		connect(action, &QAction::triggered, action, [url]() {
			execUrl(url);
		});
	} else {
		connect(action, &QAction::triggered, action, [url]() {
			auto *job = new KIO::OpenFileManagerWindowJob();
			job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
			job->setHighlightUrls({url});
			job->start();
		    KActivities::ResourceInstance::notifyAccessed(url);
		});
	}

	return action;
}

QAction*
ServiceMenuApplet::kRunnerAction(const QString &arg, QObject *parent)
{		
	auto *action = new QAction(QIcon::fromTheme(QStringLiteral("krunner"), QIcon::fromTheme(QStringLiteral("system-run"))),
							   i18n("Put Into Run Dialog"), parent);
	connect(action, &QAction::triggered, action, [arg](){
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
	});

	return action;
}

QAction*
ServiceMenuApplet::menuEditorAction(const QUrl &url, QObject *parent)
{
	const auto editorService = KService::serviceByDesktopName(QStringLiteral("org.kde.kmenuedit"));
	if (!editorService || !editorService->isValid()) {
		qWarning() << "Failed to locate kmenuedit service";
		return nullptr;
	}

	QString title, arg;
	if (const KService::Ptr service = serviceFromUrl(url)) {
		title = i18n("Edit Item");
		arg = service->menuId();
	} else if (const KServiceGroup::Ptr group = serviceGroupFromUrl(url)) {
		title = i18n("Edit Menu");
		arg = group->relPath();
	} else
		title = editorService->name();

	auto *action = new QAction(
		QIcon::fromTheme(editorService->icon(), QIcon::fromTheme(QStringLiteral("kmenuedit"))),
		escapeMnemonics(title), parent);
	connect(action, &QAction::triggered, action, [editorService, arg]() {
    	auto *job = new KIO::CommandLauncherJob(editorService->exec(),
												{!arg.isEmpty() ? arg : QStringLiteral("/")});
		job->setDesktopName(editorService->desktopEntryName());
		job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
		job->start();
	});
	return action;
}

void
ServiceMenuApplet::appendSeparatorActionToList(QList<QAction *> &actions, QObject *parent)
{
	auto *separator = new QAction(parent);
	separator->setSeparator(true);
	actions.append(separator);
}

QString
ServiceMenuApplet::escapeMnemonics(QString string)
{
	return string.replace(QLatin1Char('&'), QStringLiteral("&&"));
}
