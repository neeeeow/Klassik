/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "panelmenu.h"
#include "popuptitle.h"

#include <QMimeData>
#include <QMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QStyle>
#include <QDBusConnection>
#include <QDBusMessage>

#include <KLocalizedString>
#include <KNotificationJobUiDelegate>
#include <KProtocolManager>
#include <KFileItem>
#include <KApplicationTrader>
#include <KPropertiesDialog>
#include <KShell>
#include <KDesktopFile>

#include <KIO/ApplicationLauncherJob>
#include <KIO/CommandLauncherJob>
#include <KIO/OpenUrlJob>
#include <KIO/OpenFileManagerWindowJob>
#include <KIO/DesktopExecParser>

#include <Plasma/Applet>
#include <Plasma/Containment>
#include <Plasma/Corona>
#include <PlasmaQuick/AppletQuickItem>
#include <PlasmaActivities/ResourceInstance>
#include <PlasmaActivities/Consumer>

using namespace Qt::StringLiterals;

static KActivities::Consumer *consumer()
{
	// Returns a shared KActivities::Consumer pointer
	static QPointer<KActivities::Consumer> consumer;
	if (!consumer)
		consumer = new KActivities::Consumer(qApp);
	return consumer;
}

PanelMenu::PanelMenu(Plasma::Applet *applet, QWidget *parent)
: PanelMenu(QString(), applet, parent) {}

PanelMenu::PanelMenu(const QString &title, Plasma::Applet *applet, QWidget *parent)
: PanelMenu(QIcon(), title, applet, parent) {}

PanelMenu::PanelMenu(const QIcon &icon, const QString &title, Plasma::Applet *applet, QWidget *parent)
	: QMenu(title, parent),
	  m_applet(applet)
{
	if (!icon.isNull())
		setIcon(icon);	
	setContextMenuPolicy(Qt::CustomContextMenu);
	consumer(); // Create the shared consumer instance

	// Connect signals
	connect(this, &QWidget::customContextMenuRequested, this, &PanelMenu::showContextMenu);
	connect(this, &QMenu::aboutToShow, this, &PanelMenu::refreshContents);
	connect(this, &QMenu::aboutToHide, this, [this] {
		m_fresh = false;
		m_startPos.reset();
	});
}

Plasma::Applet *
PanelMenu::applet() const 
{
	return m_applet;
}

void
PanelMenu::refreshContents()
{
	if (m_fresh)
		return;
	if (!m_initialized) {
		initialize();
		m_initialized = true;
	}
	clear();
	populate();
	m_fresh = true;
}

void
PanelMenu::showContextMenu(const QPoint &pos)
{	
	QAction *action = actionAt(pos);
	if (!action)
		return;
	
	const QUrl url = action->data().toUrl();
	if (!url.isValid())
		return;

	auto *contextMenu = new QMenu(this);
	contextMenu->setAttribute(Qt::WA_DeleteOnClose);

	// Add contextual actions for the given QUrl
	contextMenu->addActions(PanelMenu::contextualActions(url, contextMenu));

	// Add to section
	const bool mayAddDesktop = mayAddItem(AddToTarget::Desktop);
	const bool mayAddPanel = mayAddItem(AddToTarget::Panel);
	if ((mayAddDesktop || mayAddPanel) && !contextMenu->isEmpty())
		contextMenu->addSeparator();
	
	if (mayAddDesktop) {
		QAction *desktopAction = contextMenu->addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add to Desktop"));
		connect(desktopAction, &QAction::triggered, this, [this, url]() {
			if (!addLauncher(AddToTarget::Desktop, url))
				qWarning() << "failed to add launcher to desktop!";
		});
	}
	if (mayAddPanel) {
		QAction *panelAction = contextMenu->addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add to Panel"));
		connect(panelAction, &QAction::triggered, this, [this, url]() {
			if (!addLauncher(AddToTarget::Panel, url))
				qWarning() << "failed to add launcher to panel!";
		});
	}

	if (!contextMenu->isEmpty()) {
		if (auto m = action->menu())
			m->close();
		contextMenu->popup(mapToGlobal(pos));
	} else
		delete contextMenu;
}

bool
PanelMenu::mayAddItem(AddToTarget target) const
{	
	switch (target) {
	case AddToTarget::Desktop: {				
		if (Plasma::Containment *containment = desktopContainment())
			return (containment->immutability() == Plasma::Types::Mutable);

		break;
	}
	case AddToTarget::Panel: {				
		if (Plasma::Containment *containment = panelContainment())
			return (containment->immutability() == Plasma::Types::Mutable);

		break;
	}
	}	
	
	return false;
}

bool
PanelMenu::addLauncher(AddToTarget target, const QUrl &url) const
{
	if (!mayAddItem(target)) return false;   
	
	switch (target) {
	case AddToTarget::Desktop: {		
		Plasma::Containment *containment = desktopContainment();
		if (!containment) return false;

		const QStringList containmentProvides = containment->pluginMetaData().value(u"X-Plasma-Provides", QStringList());

		if (containmentProvides.contains("org.kde.plasma.filemanagement"_L1)) {
			auto *folderQuickItem = PlasmaQuick::AppletQuickItem::itemForApplet(containment);
			if (!folderQuickItem)
				return false;

			if (QMetaObject::invokeMethod(folderQuickItem, "addLauncher", Q_ARG(QVariant, url)))
				return true;
		} else {
			// Use default plasma icon for the desktop.
			// TODO: write a custom desktop icon applet...
			if (containment->createApplet(QStringLiteral("org.kde.plasma.icon"), QVariantList() << url))
				return true;
		}

		break;
	}
	case AddToTarget::Panel: {
		if (Plasma::Containment *containment = panelContainment()) {
			if (containment->createApplet(QStringLiteral("com.github.neeeeow.klassik.panel.icon"), QVariantList() << url))
				return true;
		}

		break;
	}
	}	
	
	return false;
}

Plasma::Containment *
PanelMenu::desktopContainment() const {
	if (!m_applet) return nullptr;
	
	Plasma::Containment *containment = m_applet->containment();
	if (!containment) return nullptr;

	Plasma::Corona *corona = containment->corona();
	if (!corona) return nullptr;

	const QString activity = consumer()->currentActivity();
	return corona->containmentForScreen(containment->screen(), activity, QString());
}

Plasma::Containment *
PanelMenu::panelContainment() const {
	if (!m_applet) return nullptr;
	
	Plasma::Containment *containment = m_applet->containment();
	if (!containment ||
		containment->pluginMetaData().pluginId() != "com.github.neeeeow.klassik.panel"_L1)
		return nullptr;
	return containment;
}

void
PanelMenu::actionEvent(QActionEvent *e)
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

void
PanelMenu::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton)
		m_startPos = e->position();
	
	// We need to separately handle when a QAction with a sub menu is right clicked
	// since by default QMenu's mousePressEvent blocks contextmenus from showing if
	// a QAction has a sub menu.
	if (e->button() == Qt::RightButton && contextMenuPolicy() == Qt::CustomContextMenu) {
		if (QAction *action = actionAt(e->position().toPoint());
			action && action->menu()) {
			e->accept();
			Q_EMIT customContextMenuRequested(e->position().toPoint());				
			return;
		}
	}
	
	QMenu::mousePressEvent(e);
}

void
PanelMenu::mouseMoveEvent(QMouseEvent *e)
{
	QMenu::mouseMoveEvent(e);
	if (!(e->buttons() & Qt::LeftButton) || !m_startPos)		
		return;

	QPointF p = e->position() - *m_startPos;
	if (p.manhattanLength() <= QApplication::startDragDistance() )
		return;

	QAction *action = actionAt(m_startPos->toPoint());
	if (!action || !action->isEnabled())
		return;

	QUrl url = action->data().toUrl();
	if (!url.isValid())
		return;

	QDrag *drag = new QDrag(this);
	QMimeData *mimeData = new QMimeData;

	mimeData->setUrls({url});
	drag->setMimeData(mimeData);

	if (!action->icon().isNull()) {
		const int iconSize = style()->pixelMetric(QStyle::PM_SmallIconSize);
		drag->setPixmap(action->icon().pixmap(QSize(iconSize, iconSize), devicePixelRatio()));
	}

	if (auto m = action->menu()) // Close any submenus to avoid a weird bug
		m->close();
	m_startPos.reset();
	drag->exec(Qt::CopyAction | Qt::LinkAction);
}

void
PanelMenu::mouseReleaseEvent(QMouseEvent *e)
{
	m_startPos.reset();   
	QMenu::mouseReleaseEvent(e);	
}

QAction*
PanelMenu::noEntriesAction()
{
	auto *action = new QAction(i18n("No Entries"), this);
	action->setEnabled(false);
	return action;
}

KService::Ptr
PanelMenu::serviceFromUrl(const QUrl &url)
{
	if (!url.isValid() || !url.isLocalFile())
		return KService::Ptr();

	// First try to retrieve the service directly from the desktop path...
    KService::Ptr service = KService::serviceByDesktopPath(url.toLocalFile());
	if (service && service->isValid() && service->isApplication())
		return service;

	// ... if that fails, then we inspect the .desktop file and construct
	// a KService ourselves.
	const QString path = url.toLocalFile();
	if (!KDesktopFile::isDesktopFile(path) || !KDesktopFile::isAuthorizedDesktopFile(path))
		return KService::Ptr();

    service = KService::Ptr(new KService(path));
	if (!service || !service->isValid() || !service->isApplication())
		return KService::Ptr();
	
	return service;
}

QUrl
PanelMenu::urlFromServiceGroup(const KServiceGroup::Ptr &group)
{
	if (!group || !group->isValid())
		return QUrl();

	return urlFromServiceGroup(group->relPath());
}

QUrl
PanelMenu::urlFromServiceGroup(const QString &relPath)
{
	QUrl url;
	url.setScheme("applications"_L1);
	url.setPath(QLatin1Char('/') + relPath);

	return url;
}

KServiceGroup::Ptr
PanelMenu::serviceGroupFromUrl(const QUrl &url)
{
	if (!url.isValid() || url.scheme() != "applications"_L1
		|| !url.path().startsWith(QLatin1Char('/')))
		return KServiceGroup::Ptr();

	const KServiceGroup::Ptr group = KServiceGroup::group(url.path().mid(1));
	if (!group || !group->isValid())
		return KServiceGroup::Ptr();
	
	return group;
}

void
PanelMenu::execService(const KService::Ptr &service, const QUrl &url)
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
PanelMenu::execUrl(const QUrl &url)
{
	if (!url.isValid())
		return;
	auto *job = new KIO::OpenUrlJob(url);
	job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
	job->start();
	KActivities::ResourceInstance::notifyAccessed(url);
}

QList<QAction *>
PanelMenu::contextualActions(const QUrl &url, QObject *parent)
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
	else if (!group) {
		// And add the open with actions for any QUrl which isn't a service, service group
		KFileItem file(url);
		
	    actions.append(openWithActions(file, parent));		

		if (QAction *fileExplorerAction = openInFileExplorerAction(file, parent)) {
			appendSeparatorActionToList(actions, parent);
		    actions.append(fileExplorerAction);
		}
	}

	// Configuration section
	if (service || group) {
		// If we have a service or service group, we want to add the option to open the menu editor
		QAction *editAction = menuEditorAction(url, parent);
		if (editAction) {
			appendSeparatorActionToList(actions, parent);
			actions.append(editAction);
		}

		if (service) {
			// Furthermore for services, we add the option to put it in a KRunner dialog.
			KIO::DesktopExecParser parser(*service, {});
			const QStringList args = parser.resultingArguments();
			if (!args.isEmpty()) {
				if (QAction *runAction = kRunnerAction(KShell::joinArgs(args), parent)) {
					if (!editAction)
						appendSeparatorActionToList(actions, parent);
					actions.append(runAction);
				}
			}
		}
	} else {
		// In any other case, place a standard properties action
		appendSeparatorActionToList(actions, parent);
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
PanelMenu::jumpListActions(const KService::Ptr &service, QObject *parent)
{
	QList<QAction *> actions;	
	if (!service || !service->isValid())
		return actions;

	actions.append(new PopupTitle(QIcon::fromTheme(service->icon()), service->name(), parent));
	// We always add the option to launch the application
    auto *runAction = new QAction(QIcon::fromTheme(QStringLiteral("system-run")),
								  i18n("Run Application"), parent);
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
PanelMenu::openWithActions(const KFileItem &file, QObject *parent)
{
	QList<QAction *> actions;
	if (file.isNull() ||
		!file.url().isValid() ||
		file.url().scheme().isEmpty() ||
		file.isDir())
		return actions;

	const KService::List services = KApplicationTrader::queryByMimeType(file.mimetype());
	for (const KService::Ptr &service : services) {
		if (!service || !service->isValid() || service->noDisplay())
			continue;

		auto *action = new QAction(
			QIcon::fromTheme(service->icon(),
							 QIcon::fromTheme(QStringLiteral("application-x-executable"))),
			escapeMnemonics(service->name()), parent);

		connect(action, &QAction::triggered, action, [service, file]() {
			execService(service, file.url());
		});
		actions.append(action);
	}
	
	if (!actions.isEmpty())
		actions.prepend(new PopupTitle(QIcon::fromTheme(QStringLiteral("document-open")),
									 i18n("Open With"), parent));

	return actions;
}

QAction*
PanelMenu::openInFileExplorerAction(const KFileItem &file, QObject *parent)
{
	if (file.isNull() ||
		!file.url().isValid() ||
		file.url().scheme().isEmpty() ||
		!KProtocolManager::supportsListing(file.url()))
		return nullptr;
	
	auto *action = new QAction(QIcon::fromTheme(QStringLiteral("system-file-manager")),
							   i18n("Open in File Explorer"), parent);
	if (file.isDir()) {
		connect(action, &QAction::triggered, action, [file]() {
			execUrl(file.url());
		});
	} else {
		connect(action, &QAction::triggered, action, [file]() {
			auto *job = new KIO::OpenFileManagerWindowJob();
			job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
			job->setHighlightUrls({file.url()});
			job->start();
		});
	}

	return action;
}

QAction*
PanelMenu::kRunnerAction(const QString &arg, QObject *parent)
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

		QDBusConnection bus = QDBusConnection::sessionBus();
		bus.send(query);
	});

	return action;
}

QAction*
PanelMenu::menuEditorAction(const QUrl &url, QObject *parent)
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
    	auto *job = new KIO::CommandLauncherJob(KIO::DesktopExecParser::executablePath(editorService->exec()),
												{!arg.isEmpty() ? arg : QStringLiteral("/")});
		job->setDesktopName(editorService->desktopEntryName());
		job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
		job->start();
	});
	return action;
}

void
PanelMenu::appendSeparatorActionToList(QList<QAction *> &actions, QObject *parent)
{
	// Avoid placing a separator at the beginning, and avoid doubling separators
	if (actions.isEmpty() || actions.last()->isSeparator())
		return;
				
	auto *separator = new QAction(parent);
	separator->setSeparator(true);
	actions.append(separator);
}


QString
PanelMenu::escapeMnemonics(QString string)
{
	return string.replace(QLatin1Char('&'), QStringLiteral("&&"));
}

