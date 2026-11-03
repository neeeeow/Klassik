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

#include <KIO/ApplicationLauncherJob>
#include <KIO/CommandLauncherJob>
#include <KIO/OpenUrlJob>
#include <KIO/OpenFileManagerWindowJob>

#include <PlasmaActivities/ResourceInstance>
#include <PlasmaQuick/AppletQuickItem>

ServiceMenuApplet::ServiceMenuApplet(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args)
	: Plasma::Applet(parentObject, data, args) {}

ServiceMenuApplet::~ServiceMenuApplet()
{
	deleteMenu(false); // don't send signals in the destructor
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

void
ServiceMenuApplet::initMenu()
{
	// Delete any old menu objects
	deleteMenu(false);

	// Create the menu object. If it fails to create, simply bail out.
	m_menu = createMenu();
	if (!m_menu)
		return;

	// Let the applet know the menu's visiblity has changed
	connect(m_menu, &QMenu::aboutToShow, this, &ServiceMenuApplet::menuActiveChanged, Qt::QueuedConnection);
	connect(m_menu, &QMenu::aboutToHide, this, &ServiceMenuApplet::menuActiveChanged, Qt::QueuedConnection);

	// Reconfigure the menu when the timer changes
	if (m_timer)
		connect(m_timer, &QTimer::timeout, m_menu, &ServiceMenu::reinitialize);

	Q_EMIT hasMenuChanged();
	Q_EMIT menuActiveChanged();
}

void
ServiceMenuApplet::deleteMenu(bool notify)
{
	if (!m_menu)
		return;

	m_menu->disconnect(this);
	m_menu->close();
	m_menu->deleteLater();
	m_menu.clear();
	if (notify) {
		Q_EMIT hasMenuChanged();
		Q_EMIT menuActiveChanged();
	}
}

bool
ServiceMenuApplet::menuActive() const
{
	return m_menu && m_menu->isVisible();
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

void
ServiceMenuApplet::notifyAccessed(const QUrl &url) const
{
	if (!url.isValid())
		return;
	KActivities::ResourceInstance::notifyAccessed(
		url, pluginName() );	
}

void
ServiceMenuApplet::menuEditor(const QString &arg) const
{
	const auto service = KService::serviceByDesktopName(QStringLiteral("org.kde.kmenuedit"));
	if (!service) {
		qWarning() << "Could not find kmenuedit";
		return;
	}	

	auto *job = new KIO::CommandLauncherJob(service->exec(),
											{!arg.isEmpty() ? arg : QStringLiteral("/")});
	job->setDesktopName(service->desktopEntryName());
	job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoErrorHandlingEnabled));
	job->start();
	notifyAccessed(QUrl(QStringLiteral("applications:") + service->storageId()));
}

void
ServiceMenuApplet::invokeKRunner(const QString &arg)
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

QList<QAction *>
ServiceMenuApplet::jumpListActions(const KService::Ptr &service, QObject *parent) const
{
	QList<QAction *> actions;	
	if (!service || !service->isValid())
		return actions;

	const QList<KServiceAction> serviceActions = service->actions();
	for (const KServiceAction &serviceAction : serviceActions) {
		if (serviceAction.noDisplay())
			continue;

		auto *action = new QAction(QIcon::fromTheme(serviceAction.icon()),
								   serviceAction.text().replace(QLatin1Char('&'), QStringLiteral("&&")), parent);
		if (serviceAction.isSeparator())
			action->setSeparator(true);

		connect(action, &QAction::triggered, this, [this, service, serviceAction]() {
			auto *job = new KIO::ApplicationLauncherJob(serviceAction);
			job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoErrorHandlingEnabled));
			job->start();
			notifyAccessed(QUrl(QStringLiteral("applications:") + service->storageId()));
		});

		actions.append(action);
	}

	if (!actions.isEmpty())
		actions.prepend(new PopupTitle(QIcon::fromTheme(service->icon()), service->name(), parent));

	return actions;
}

QList<QAction *>
ServiceMenuApplet::openWithActions(const QUrl &url, QObject *parent) const
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
			service->name().replace(QLatin1Char('&'), QStringLiteral("&&")), parent);

		connect(action, &QAction::triggered, this, [this, service, url]() {
			auto *job = new KIO::ApplicationLauncherJob(service);
			job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoErrorHandlingEnabled));
			job->setUrls({url});
			job->start();
			notifyAccessed(url);
		});
		actions.append(action);
	}
	
	if (!actions.isEmpty())
		actions.prepend(new PopupTitle(QIcon::fromTheme(QStringLiteral("document-open")),
									 i18n("Open With"), parent));

	return actions;
}

QAction*
ServiceMenuApplet::openInFileExplorerAction(const QUrl &url, QObject *parent) const
{
	if (!url.isValid() || url.scheme().isEmpty() ||
		!KProtocolManager::supportsListing(url))
		return nullptr;
	
	auto *action = new QAction(QIcon::fromTheme(QStringLiteral("system-file-manager")),
							   i18n("Open in File Explorer"), parent);
	if (KFileItem(url).isDir()) {
		connect(action, &QAction::triggered, this, [this, url]() {
			auto *job = new KIO::OpenUrlJob(url);
			job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoErrorHandlingEnabled));
			job->start();
			notifyAccessed(url);
		});
	} else {
		connect(action, &QAction::triggered, this, [this, url]() {
			auto *job = new KIO::OpenFileManagerWindowJob();
			job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoErrorHandlingEnabled));
			job->setHighlightUrls({url});
			job->start();
			notifyAccessed(url);
		});
	}

	return action;
}
