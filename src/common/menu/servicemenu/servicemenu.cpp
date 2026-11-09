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
#include <KFileItem>

ServiceMenu::ServiceMenu(ServiceMenuApplet *applet, QWidget *parent)
: ServiceMenu(QString(), applet, parent) {}

ServiceMenu::ServiceMenu(const QString &title, ServiceMenuApplet *applet, QWidget *parent)
: ServiceMenu(QIcon(), title, applet, parent) {}

ServiceMenu::ServiceMenu(const QIcon &icon, const QString &title, ServiceMenuApplet *applet, QWidget *parent)
	: QMenu(title, parent),
	  m_applet(applet)
{
	if (!icon.isNull())
		setIcon(icon);	
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &QWidget::customContextMenuRequested, this, &ServiceMenu::showContextMenu);
	connect(this, &QMenu::aboutToShow, this, &ServiceMenu::refreshContents);
	connect(this, &QMenu::aboutToHide, this, [this] {
		m_fresh = false;
		m_startPos.reset();
	});
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
	if (!service || !service->isValid())
		return nullptr;

	QString title = ServiceMenuApplet::escapeMnemonics(service->name());
	if (m_displayGenericName) {
		const QString genericName = ServiceMenuApplet::escapeMnemonics(service->genericName());
		if (!genericName.isEmpty() && genericName != title)
			title.append(QStringLiteral(" (%1)").arg(genericName));
	}
	
	auto *action = new QAction(
		QIcon::fromTheme(service->icon(),
						 QIcon::fromTheme(QStringLiteral("application-x-executable"))),
		title, this);

	action->setData(QUrl::fromLocalFile(service->entryPath())); // Store the URL to the .desktop entry
	action->setToolTip(service->comment()); // Set the action tooltip
	
	connect(action, &QAction::triggered, action, [service, url]() {
		ServiceMenuApplet::execService(service, url);
	});

	return action;
}

QAction*
ServiceMenu::createActionFromUrl(const QUrl &url)
{
	if (!url.isValid())
		return nullptr;	

	KFileItem fileItem(url);
	const QIcon icon = QIcon::fromTheme(fileItem.iconName(), QIcon::fromTheme(QStringLiteral("text-x-generic")));

	return createActionFromUrl(url, icon, fileItem.text());
}

QAction*
ServiceMenu::createActionFromUrl(const QUrl &url, const QIcon &icon, const QString &title)
{
	if (!url.isValid())
		return nullptr;

	auto *action = new QAction(icon, ServiceMenuApplet::escapeMnemonics(title), this);
	action->setData(url);
	action->setToolTip(url.toDisplayString(QUrl::PreferLocalFile));
	
	connect(action, &QAction::triggered, action, [url]() {
		ServiceMenuApplet::execUrl(url);
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

	const QUrl url = action->data().toUrl();
	if (!url.isValid())
		return;

	QMenu contextMenu; // No need for a parent since we're declaring on the stack
	contextMenu.addActions(ServiceMenuApplet::contextualActions(url, &contextMenu));
	
	if (!contextMenu.isEmpty())
		contextMenu.addSeparator();

	// Add to section
	// Actions for adding the item to the desktop/panel
	if (containmentInterface->mayAddItem(ContainmentInterface::Desktop)) {
		QAction *desktopAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add to Desktop"));
		connect(desktopAction, &QAction::triggered, containmentInterface, [containmentInterface, url]() {
			containmentInterface->addLauncher(ContainmentInterface::Desktop, url);
		});
	}
	if (containmentInterface->mayAddItem(ContainmentInterface::Panel)) {
		QAction *panelAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add to Panel"));
		connect(panelAction, &QAction::triggered, containmentInterface, [containmentInterface, url]() {
			containmentInterface->addLauncher(ContainmentInterface::Panel, url);
		});
	}

	if (!contextMenu.isEmpty())
		contextMenu.exec(mapToGlobal(pos));
}

/* Mouse events adapted from KDE 3.5 kicker source code.
   Copyright (c) 1996-2000 the KDE 3 kicker authors.
   Source available: https://kde.org/info/1-2-3/3.5.10/ */

void
ServiceMenu::mousePressEvent(QMouseEvent *e)
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
ServiceMenu::mouseMoveEvent(QMouseEvent *e)
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

	QPointer<ServiceMenu> guard(this); // guard in case the menu destroys itself
	if (auto m = action->menu()) // Close any submenus to avoid a weird bug
		m->close();
	drag->exec(Qt::CopyAction | Qt::LinkAction);
	if (!guard) return;
	m_startPos.reset();
}

void
ServiceMenu::mouseReleaseEvent(QMouseEvent *e)
{
	m_startPos.reset();   
	QMenu::mouseReleaseEvent(e);	
}
