#include "popupmenu.h"

#include <QList>
#include <QMimeData>
#include <QMimeDatabase>
#include <QMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QStyle>

#include <KLocalizedString>
#include <KNotificationJobUiDelegate>

#include <KIO/ApplicationLauncherJob>
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
PopupMenu::createActionFromService(const KService::Ptr &service)
{
	// Create the menu item itself. Note, we use .replace(QStringLiteral("&"), QStringLiteral("&&")) to ensure that ampersands
	// don't inadvertently get interpreted as mnemonics. There is probably an easier way to do this, but it works!
	QAction *action = new QAction(QIcon::fromTheme(service->icon()), service->name().replace(QStringLiteral("&"), QStringLiteral("&&")), this);

	// Find the url and save it
	QUrl url = QUrl::fromLocalFile(service->entryPath());
	action->setData(url);
	
	connect(action, &QAction::triggered, this, [service, url]() {
		auto *job = new KIO::ApplicationLauncherJob(service);
		job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
		job->start();
		KActivities::ResourceInstance::notifyAccessed(
			QUrl(QStringLiteral("applications:") + service->storageId()),
			QStringLiteral("com.github.neeeeow.klassik.kmenu")
			);			
	});

	return action;
}

QAction*
PopupMenu::createActionFromUrl(const QUrl &url)
{
	QString fileName = url.fileName().replace(QStringLiteral("&"), QStringLiteral("&&")); // name to display in the menu
		
	QMimeDatabase db; // use QMimeDatabase to fetch the icon name
	QMimeType mime = db.mimeTypeForUrl(url);
	QIcon icon = QIcon::fromTheme(mime.iconName());

	QAction *action = new QAction(icon, fileName, this);
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

void
PopupMenu::showContextMenu(const QPoint &pos)
{
	QAction *action = actionAt(pos);
	if (!action)
		return;

	QUrl url = action->data().toUrl();
	if (!url.isValid())
		return;

	QMenu contextMenu(this);

	QAction *addPanelAction = new QAction(
        QIcon::fromTheme(QStringLiteral("kicker")), 
        i18n("Add File to Main Panel"), 
        &contextMenu
    );

	contextMenu.addAction(addPanelAction); 
	
	/*if (url.scheme() == QStringLiteral("applications")) {
		// We need to discriminate between KService and KServiceGroup now
	} else {	   
	}*/

	// Display the menu
	QAction *selectedAction = contextMenu.exec(mapToGlobal(pos));

	/*if (selectedAction == addPanelAction) {

		}*/
}

/* Mouse events adapted from KDE 3.5 kicker source code.
   Copyright (c) 1996-2000 the KDE 3 kicker authors.
   Source available: https://kde.org/info/1-2-3/3.5.10/ */

void
PopupMenu::mousePressEvent(QMouseEvent *ev)
{
	if (ev->button() == Qt::LeftButton)
		m_startPos = ev->position();
	
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
	if (!url.isValid())
		return;

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
