#include "popupmenu.h"

#include <QList>
#include <QMimeDatabase>

#include <KIO/ApplicationLauncherJob>
#include <KIO/OpenUrlJob>
#include <KIO/JobUiDelegateFactory>

#include <PlasmaActivities/ResourceInstance>

PopupMenu::PopupMenu(QWidget *parent)
	: QMenu(parent)
{
}

PopupMenu::PopupMenu(const QString &title, QWidget *parent)
	: QMenu(title, parent) 
{
}

PopupMenu::~PopupMenu() = default;

void
PopupMenu::cleanupActionList(QList<QAction *> &actionList, QMenu *menu)
{	
	if (!menu)
		menu = this;
	
	// Cleans up all member actions of our QList from the menu
	for (QAction *action : actionList) {
		menu->removeAction(action);
	}

	// Clear out the list itself
	qDeleteAll(actionList);
	actionList.clear();
}

QAction*
PopupMenu::createActionFromService(KService::Ptr service, QObject *parent)
{
	if (!parent)
		parent = this;

	// Create the menu item itself. Note, we use .replace(QStringLiteral("&"), QStringLiteral("&&")) to ensure that ampersands
	// don't inadvertently get interpreted as mnemonics. There is probably an easier way to do this, but it works!
	QAction *action = new QAction(QIcon::fromTheme(service->icon()), service->name().replace(QStringLiteral("&"), QStringLiteral("&&")), this);

	// Find the url and save it
	QUrl url(service->storageId());
	url.setScheme(QStringLiteral("applications"));
	action->setData(url);
	
	connect(action, &QAction::triggered, parent, [service, url]() {
		auto *job = new KIO::ApplicationLauncherJob(service);
		job->setUiDelegate(KIO::createDefaultJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, nullptr));
		job->start();
		KActivities::ResourceInstance::notifyAccessed(
			url,
			QStringLiteral("com.github.neeeeow.klassik.kmenu")
			);			
	});

	return action;
}

QAction*
PopupMenu::createActionFromUrl(QUrl url, QObject *parent)
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
		job->setUiDelegate(KIO::createDefaultJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, nullptr));
		job->start();
		KActivities::ResourceInstance::notifyAccessed(
			url,
			QStringLiteral("com.github.neeeeow.klassik.kmenu")
			);			
	});

	return action;
}
