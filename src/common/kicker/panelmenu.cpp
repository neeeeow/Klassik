/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "panelmenu.h"
#include "iteminfo.h"
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
#include <KApplicationTrader>
#include <KPropertiesDialog>
#include <KShell>
#include <KConfigLoader>

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

PanelMenu::PanelMenu(const ItemInfo &info, Plasma::Applet *applet, QWidget *parent)
	: PanelMenu(QIcon::fromTheme(info.icon()), info.name(), applet, parent) {}

PanelMenu::PanelMenu(const QString &title, Plasma::Applet *applet, QWidget *parent)
	: PanelMenu(QIcon(), title, applet, parent) {}

PanelMenu::PanelMenu(const QIcon &icon, const QString &title, Plasma::Applet *applet, QWidget *parent)
	: QMenu(escapeMnemonics(title), parent),
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
	
	const ItemInfo info = action->data().value<ItemInfo>();
	if (!info.isValid())
		return;

	auto *contextMenu = new QMenu(this);
	contextMenu->setAttribute(Qt::WA_DeleteOnClose);

	// Add contextual actions for the given QUrl
	contextMenu->addActions(PanelMenu::contextualActions(info, contextMenu));

	// Add to section
	const bool mayAddDesktop = mayAddItem(AddToTarget::Desktop);
	const bool mayAddPanel = mayAddItem(AddToTarget::Panel);
	if ((mayAddDesktop || mayAddPanel) && !contextMenu->isEmpty())
		contextMenu->addSeparator();
	
	if (mayAddDesktop) {
		QAction *desktopAction = contextMenu->addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add to Desktop"));
		connect(desktopAction, &QAction::triggered, this, [this, url = info.url()]() {
			if (!addLauncher(AddToTarget::Desktop, url))
				qWarning() << "failed to add launcher to desktop!";
		});
	}
	if (mayAddPanel) {
		QAction *panelAction = contextMenu->addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add to Panel"));
		connect(panelAction, &QAction::triggered, this, [this, url = info.url()]() {
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

	const ItemInfo info = action->data().value<ItemInfo>();
	if (!info.isValid())
		return;

	QDrag *drag = new QDrag(this);
	QMimeData *mimeData = new QMimeData;

	mimeData->setUrls({info.url()});
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

void
PanelMenu::execService(const KService::Ptr &service, const QUrl &url)
{
	if (!service || !service->isValid()) {
		qWarning() << "attempted to exec invalid service!";
		return;
	}
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
	if (!url.isValid()) {
		qWarning() << "attempted to exec invalid url!";
		return;
	}
	auto *job = new KIO::OpenUrlJob(url);
	job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
	job->start();
	KActivities::ResourceInstance::notifyAccessed(url);
}

QList<QAction *>
PanelMenu::contextualActions(const ItemInfo &info, QObject *parent)
{
	QList<QAction *> actions;
	if (!info.isValid())
		return actions;

	switch (info.type()) {
	case ItemInfo::Type::Service: {
		const KService::Ptr service = info.service();
		if (!service)
			break;

		// Jump list actions
		actions.append(jumpListActions(service, parent));

		// Menu editor action
		QAction *editAction = menuEditorAction(info, parent);
		if (editAction) {
			appendSeparatorActionToList(actions, parent);
			actions.append(editAction);
		}

		// KRunner action
		KIO::DesktopExecParser parser(*service, {});
		const QStringList args = parser.resultingArguments();
		if (!args.isEmpty()) {
			if (QAction *runAction = kRunnerAction(KShell::joinArgs(args), parent)) {
				if (!editAction)
					appendSeparatorActionToList(actions, parent);
				actions.append(runAction);
			}
		}
		
		break;
	}
	case ItemInfo::Type::ServiceGroup: {
		const KServiceGroup::Ptr group = info.group();
		if (!group)
			break;

		if (QAction *editAction = menuEditorAction(info, parent))
			actions.append(editAction);
		
		break;
	}
	case ItemInfo::Type::File:
	case ItemInfo::Type::Directory: {
		actions.append(openWithActions(info, parent));		
		if (QAction *fileExplorerAction = openInFileExplorerAction(info, parent)) {
			appendSeparatorActionToList(actions, parent);
		    actions.append(fileExplorerAction);
		}

		appendSeparatorActionToList(actions, parent);
		auto *propertiesAction = new QAction(QIcon::fromTheme(QStringLiteral("document-properties")), i18n("Properties"), parent);
		connect(propertiesAction, &QAction::triggered, propertiesAction, [url = info.url()]() {
			auto *dlg = new KPropertiesDialog(url);
			dlg->setAttribute(Qt::WA_DeleteOnClose);
			dlg->setFileNameReadOnly(true);
			dlg->setWindowIcon(QIcon::fromTheme(QStringLiteral("document-properties")));
			dlg->show();
		});
		actions.append(propertiesAction);
		
		break;
	}
	case ItemInfo::Type::Invalid: {
		break;
	}
	}
	
	return actions;
}

QList<QAction *>
PanelMenu::jumpListActions(const KService::Ptr &service, QObject *parent)
{
	QList<QAction *> actions;
	
	ItemInfo info(service);
	if (!info.isValid())
		return actions;

	actions.append(new PopupTitle(QIcon::fromTheme(info.icon()), info.name(), parent));
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
PanelMenu::openWithActions(const ItemInfo &info, QObject *parent)
{
	QList<QAction *> actions;
	if (info.type() != ItemInfo::Type::File)
		return actions;

	const KService::List services = KApplicationTrader::queryByMimeType(info.mimetype());
	for (const KService::Ptr &service : services) {		
		if (!service || !service->isValid() || service->noDisplay())
			continue;

		ItemInfo serviceInfo(service);
		if (!serviceInfo.isValid())
			continue;

		auto *action = new QAction(QIcon::fromTheme(serviceInfo.icon()),
								   escapeMnemonics(serviceInfo.name()), parent);
		connect(action, &QAction::triggered, action, [service, url = info.url()]() {
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
PanelMenu::openInFileExplorerAction(const ItemInfo &info, QObject *parent)
{
	if (!info.isValid() ||
		!KProtocolManager::supportsListing(info.url()))
		return nullptr;
	
	auto *action = new QAction(QIcon::fromTheme(QStringLiteral("system-file-manager")),
							   i18n("Open in File Explorer"), parent);
	const QUrl url = info.url();
	if (info.type() == ItemInfo::Type::Directory) {
		connect(action, &QAction::triggered, action, [url]() {
			execUrl(url);
		});
	} else {
		connect(action, &QAction::triggered, action, [url]() {
			auto *job = new KIO::OpenFileManagerWindowJob();
			job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
			job->setHighlightUrls({url});
			job->start();
		});
	}

	return action;
}

QAction*
PanelMenu::kRunnerAction(QObject *parent)
{
	return kRunnerAction(QString(), parent);
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
PanelMenu::menuEditorAction(QObject *parent)
{
	return menuEditorAction(ItemInfo(), parent);
}

QAction*
PanelMenu::menuEditorAction(const ItemInfo &info, QObject *parent)
{
	const auto editorService = KService::serviceByDesktopName(QStringLiteral("org.kde.kmenuedit"));
	if (!editorService || !editorService->isValid()) {
		qWarning() << "Failed to locate kmenuedit service";
		return nullptr;
	}

	QString title, arg;	
	if (const KService::Ptr service = info.service()) {
		title = i18n("Edit Item");
		arg = service->menuId();
	} else if (const KServiceGroup::Ptr group = info.group()) {
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

QVariant
PanelMenu::configValue(const QString &key) const
{
	if (!m_applet)
		return QVariant();

	KConfigLoader *configLoader = m_applet->configScheme();
	if (!configLoader)
		return QVariant();

	const QVariant value = configLoader->property(key);
	if (!value.isValid()) {
		qWarning() << "key" << key << "not found!";
		return QVariant();
	}
	return value;
}

