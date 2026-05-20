#include "popupmenu.h"
#include "popupmenutitle.h"

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
#include <KServiceGroup>
#include <KFileItem>
#include <KApplicationTrader>

#include <KIO/ApplicationLauncherJob>
#include <KIO/CommandLauncherJob>
#include <KIO/OpenUrlJob>

#include <PlasmaActivities/ResourceInstance>

PopupMenu::PopupMenu(Plasma::Containment *containment, QWidget *parent)
	: QMenu(parent)
{
	m_containment = new ContainmentInterface(containment, this);
	initialize();
}

PopupMenu::PopupMenu(const QString &title, Plasma::Containment *containment, QWidget *parent)
	: QMenu(title, parent)
{
	m_containment = new ContainmentInterface(containment, this);
	initialize();
}

PopupMenu::~PopupMenu() = default;

void
PopupMenu::initialize()
{	
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &QMenu::customContextMenuRequested, this, &PopupMenu::showContextMenu);   
}

void
PopupMenu::cleanupActionList(QList<QAction *> &actionList)
{		
	// Cleans up all member actions of our QList from the menu
	for (QAction *action : actionList) {
		removeAction(action);
	}

	// Clear out the list itself
	qDeleteAll(actionList);
	actionList.clear();
}

QAction*
PopupMenu::createActionFromService(const KService::Ptr &service, const QUrl &url, QWidget *parent)
{
	/* parameters:
	       service: the KService to launch
		   url: the QUrl of any files the service should open
	*/
	if (!parent)
		parent = this;
	
	QAction *action = new QAction(QIcon::fromTheme(service->icon()), service->name().replace(QStringLiteral("&"), QStringLiteral("&&")), parent);

    action->setData(QVariant::fromValue(service)); // Store the KService
	
	connect(action, &QAction::triggered, parent, [service, url]() {
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
PopupMenu::createActionFromUrl(const QUrl &url, QWidget *parent)
{
	if (!parent)
		parent = this;
	
	QString fileName = url.fileName().replace(QStringLiteral("&"), QStringLiteral("&&")); // name to display in the menu
		
	QMimeDatabase db; // use QMimeDatabase to fetch the icon name
	QMimeType mime = db.mimeTypeForUrl(url);
	QIcon icon = QIcon::fromTheme(mime.iconName());

	QAction *action = new QAction(icon, fileName, parent);
	action->setData(url);
	
	connect(action, &QAction::triggered, parent, [url]() {
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

void
PopupMenu::showContextMenu(const QPoint &pos)
{
	QAction *action = actionAt(pos);
	if (!action)
		return;

	QMenu contextMenu(this);

	if (action->data().canConvert<KService::Ptr>()) {
		// KService means application!
		KService::Ptr service = action->data().value<KService::Ptr>();
		
		if (m_containment->mayAddLauncher(ContainmentInterface::Desktop)) {
			QAction *desktopAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add to Desktop"));
			connect(desktopAction, &QAction::triggered, m_containment, [this, service]() {
				m_containment->addLauncher(ContainmentInterface::Desktop, service);
			});
		}

		if (m_containment->mayAddLauncher(ContainmentInterface::TaskManager)) {
			if (!m_containment->hasLauncher(ContainmentInterface::TaskManager, service)) {
				QAction *taskManagerAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("pin")), i18n("Pin to Task Manager"));
				connect(taskManagerAction, &QAction::triggered, m_containment, [this, service]() {
					m_containment->addLauncher(ContainmentInterface::TaskManager, service);
				});
			}
		}

		if (m_containment->mayAddLauncher(ContainmentInterface::Panel)) {
			QAction *panelAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add to Panel"));
			connect(panelAction, &QAction::triggered, m_containment, [this, service]() {
				m_containment->addLauncher(ContainmentInterface::Panel, service);
			});
		}

		if (!contextMenu.isEmpty())
			contextMenu.addSeparator();

		QAction *editAction = contextMenu.addAction(QIcon::fromTheme(QStringLiteral("kmenuedit")), i18n("Edit Application"));
		connect(editAction, &QAction::triggered, &contextMenu, [this, service]() {
			runMenuEditor(service->menuId());
		});
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
			contextMenu.addAction(new PopupMenuTitle(i18n("Open with"), &contextMenu));

			for (const KService::Ptr &service : services) {
				QAction *action = createActionFromService(service, url, &contextMenu);
				contextMenu.addAction(action);
			}						
		} else
			return;
	} else
		return;

	// Display the menu
	contextMenu.exec(mapToGlobal(pos));
}

void
PopupMenu::runMenuEditor(QString arg)
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
PopupMenu::invokeKRunner()
{
	QDBusInterface krunner(QStringLiteral("org.kde.krunner"), QStringLiteral("/App"), QStringLiteral("org.kde.krunner.App"));

	krunner.call(QStringLiteral("query"), QStringLiteral(""));

	krunner.call(QStringLiteral("display"));
}

/* Mouse events adapted from KDE 3.5 kicker source code.
   Copyright (c) 1996-2000 the KDE 3 kicker authors.
   Source available: https://kde.org/info/1-2-3/3.5.10/ */

void
PopupMenu::mousePressEvent(QMouseEvent *ev)
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
PopupMenu::mouseMoveEvent(QMouseEvent *ev)
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
