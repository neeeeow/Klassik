/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "servicemenu.h"
#include "servicemenuapplet.h"
#include "containmentinterface.h"

#include <QMimeData>
#include <QMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QStyle>

#include <KLocalizedString>
#include <KNotificationJobUiDelegate>
#include <KPropertiesDialog>
#include <KServiceGroup>
#include <KFileItem>

#include <KIO/ApplicationLauncherJob>
#include <KIO/OpenUrlJob>
#include <KIO/DesktopExecParser>

ServiceMenu::ServiceMenu(ServiceMenuApplet *applet, QWidget *parent)
: ServiceMenu(QIcon(), QString(), applet, parent) {}

ServiceMenu::ServiceMenu(const QString &title, ServiceMenuApplet *applet, QWidget *parent)
: ServiceMenu(QIcon(), title, applet, parent) {}

ServiceMenu::ServiceMenu(const QIcon &icon, const QString &title, ServiceMenuApplet *applet, QWidget *parent)
	: QMenu(parent),
	  m_applet(applet)
{
	if (!icon.isNull())
		setIcon(icon);
	if (!title.isEmpty())
		setTitle(title);	
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &QWidget::customContextMenuRequested, this, &ServiceMenu::showContextMenu);
	connect(this, &QMenu::aboutToShow, this, &ServiceMenu::refreshContents);
	connect(this, &QMenu::aboutToHide, this, [this] { m_fresh = false; });
	connect(this, &QMenu::aboutToHide, this, [this] { m_startPos.reset(); }); 
}

void
ServiceMenu::initialize()
{
	if (m_initialized) return;
	loadConfig();
	rebuild();	
	m_initialized = true;
}

void
ServiceMenu::refreshContents()
{
	if (m_fresh)
		return;
	if (!m_initialized)
		initialize();
	clear();
	populate();
	m_fresh = true;
}

void
ServiceMenu::loadConfig()
{
	m_displayGenericName = applet() ? applet()->getConfigValue<bool>(QStringLiteral("showGenericName")) : true;
	setToolTipsVisible(applet() ? applet()->getConfigValue<bool>(QStringLiteral("showTooltips")) : false);
}

ServiceMenuApplet*
ServiceMenu::applet() const
{
	return m_applet;
}

void
ServiceMenu::actionEvent(QActionEvent *e)
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
ServiceMenu::createActionFromService(const KService::Ptr &service, const QUrl &url)
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
		job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoErrorHandlingEnabled));
		if (!url.isEmpty())
			job->setUrls({url});
		job->start();
		if (auto a = applet())
			a->notifyAccessed(QUrl(QStringLiteral("applications:") + service->storageId()));
	});

	return action;
}

QAction*
ServiceMenu::createActionFromUrl(const QUrl &url)
{
	/* Returns a QAction* which launches a given QUrl, parented to this. */
	if (!url.isValid())
		return nullptr;	

	KFileItem fileItem(url);
	const QString fileName = escapeMnemonics(fileItem.text());
	const QIcon icon = QIcon::fromTheme(fileItem.iconName(), QIcon::fromTheme(QStringLiteral("text-x-generic")));

	auto *action = new QAction(icon, fileName, this);
	action->setData(url);
	action->setToolTip(url.toDisplayString(QUrl::PreferLocalFile));
	
	connect(action, &QAction::triggered, this, [this, url]() {
		auto *job = new KIO::OpenUrlJob(url);
		job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoErrorHandlingEnabled));
		job->start();
		if (auto a = applet())
			a->notifyAccessed(url);
	});

	return action;
}

void
ServiceMenu::showContextMenu(const QPoint &pos) const
{
	ServiceMenuApplet *a = applet();
	if (!a)
		return;
	ContainmentInterface *containmentInterface = a->containmentInterface();
	if (!containmentInterface)
		return; // should never happen, but just in case
	
	QAction *action = actionAt(pos);
	if (!action)
		return;

	QMenu contextMenu; // No need for a parent since we're declaring on the stack
	contextMenu.setContextMenuPolicy(Qt::NoContextMenu); // disallow the contextmenu to spawn additional context menus

	if (action->data().canConvert<KService::Ptr>()) {
		// KService means application!
		const KService::Ptr service = action->data().value<KService::Ptr>();
		if (!service || !service->isValid())
			return;

		contextMenu.addActions(a->jumpListActions(service, &contextMenu));
		if (!contextMenu.isEmpty())
			contextMenu.addSeparator();

		// Actions for adding the item to the desktop/panel
		if (containmentInterface->mayAddItem(ContainmentInterface::Desktop)) {
			QAction *desktopAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add Item to Desktop"));
			connect(desktopAction, &QAction::triggered, containmentInterface, [containmentInterface, service]() {
				containmentInterface->addLauncher(ContainmentInterface::Desktop, service);
			});
		}
		if (containmentInterface->mayAddItem(ContainmentInterface::Panel)) {
			QAction *panelAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add Item to Panel"));
			connect(panelAction, &QAction::triggered, containmentInterface, [containmentInterface, service]() {
				containmentInterface->addLauncher(ContainmentInterface::Panel, service);
			});
		}

		if (!contextMenu.isEmpty())
			contextMenu.addSeparator();

		// Action for editing the menu item
		QAction *editAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("kmenuedit")), i18n("Edit Item"));
		connect(editAction, &QAction::triggered, a, [a, service]() {
			a->menuEditor(service->menuId());
		});

		// Action to put the action's exec command in krunner
		QAction *runAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("system-run")), i18n("Put Into Run Dialog"));
		connect(runAction, &QAction::triggered, a, [a, service](){
			a->invokeKRunner(KIO::DesktopExecParser::executablePath(service->exec()));
		});
		
	} else if (action->data().canConvert<KServiceGroup::Ptr>()) {
		// KServiceGroup means sub menu container
		const KServiceGroup::Ptr group = action->data().value<KServiceGroup::Ptr>();
		if (!group || !group->isValid())
			return;

		if (containmentInterface->mayAddItem(ContainmentInterface::Panel)) {
			QAction *panelAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add Menu to Panel"));
			connect(panelAction, &QAction::triggered, containmentInterface, [containmentInterface, group]() {
				containmentInterface->addServiceGroupMenu(group);
			});
			contextMenu.addSeparator();
		}
		
		QAction *editAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("kmenuedit")), i18n("Edit Menu"));
		connect(editAction, &QAction::triggered, a, [a, group]() {
			a->menuEditor(group->relPath());
		});
	} else if (action->data().canConvert<QUrl>()) {
		// QUrl means a file path
		const QUrl url = action->data().toUrl();
		if (!url.isValid())
			return;

		contextMenu.addActions(a->openWithActions(url, &contextMenu));
		if (!contextMenu.isEmpty())
			contextMenu.addSeparator();
			
		QAction *fileExplorerAction = a->openInFileExplorerAction(url, &contextMenu);
		if (fileExplorerAction) {						
			contextMenu.addAction(fileExplorerAction);
			contextMenu.addSeparator();
		}
		
		QAction *propertiesAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("document-properties")), i18n("Properties"));
		connect(propertiesAction, &QAction::triggered, &contextMenu, [url]() {
			auto *dlg = new KPropertiesDialog(url);
			dlg->setAttribute(Qt::WA_DeleteOnClose);
			dlg->setFileNameReadOnly(true);
			dlg->setWindowIcon(QIcon::fromTheme(QStringLiteral("document-properties")));
			dlg->show();
		});	   
		
	} else
		return;

	// Display the menu
	if (!contextMenu.isEmpty())
		contextMenu.exec(mapToGlobal(pos));
}

QString
ServiceMenu::escapeMnemonics(QString string)
{
	return string.replace(QLatin1Char('&'), QStringLiteral("&&"));
}

/* Mouse events adapted from KDE 3.5 kicker source code.
   Copyright (c) 1996-2000 the KDE 3 kicker authors.
   Source available: https://kde.org/info/1-2-3/3.5.10/ */

void
ServiceMenu::mousePressEvent(QMouseEvent *ev)
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
ServiceMenu::mouseMoveEvent(QMouseEvent *ev)
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
			if (!service || service->entryPath().isEmpty())
				return;
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

	QPointer<ServiceMenu> guard(this); // guard in case the menu destroys itself
	drag->exec(Qt::CopyAction | Qt::LinkAction);
	if (!guard) return;
	m_startPos.reset();
}

void
ServiceMenu::mouseReleaseEvent(QMouseEvent *ev)
{
	m_startPos.reset();
	QMenu::mouseReleaseEvent(ev);	
}
