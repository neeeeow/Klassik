#include "containmentinterface.h"

#include <Plasma/Corona>
#include <PlasmaQuick/AppletQuickItem>

ContainmentInterface::ContainmentInterface(Plasma::Containment *containment, QObject *parent)
	: QObject(parent)
{
	m_containment = containment;
}

ContainmentInterface::~ContainmentInterface() = default;

bool
ContainmentInterface::mayAddLauncher(ContainmentInterface::Target target)
{
	Plasma::Corona *corona = m_containment->corona();
	if (!corona)
		return false;

	switch (target) {
	case Desktop: {		
		Plasma::Containment *containment = corona->containmentForScreen(m_containment->screen(), QString(), QString());
		if (containment)
			return (containment->immutability() == Plasma::Types::Mutable);       

        break;
	}
	case Panel: {
		if (m_containment->pluginMetaData().pluginId() == QLatin1String("org.kde.panel"))
			return (m_containment->immutability() == Plasma::Types::Mutable);

		break;
    }
	case TaskManager: {
		if (m_containment->pluginMetaData().pluginId() == QLatin1String("org.kde.panel")) {
			auto *taskManager = findTaskManagerApplet();
			if (!taskManager)
				return false;

			auto *taskManagerQuickItem = PlasmaQuick::AppletQuickItem::itemForApplet(taskManager);
			if (!taskManagerQuickItem)
				return false;
			
			return taskManagerQuickItem->property("supportsLaunchers").toBool();
        }

        break;
    }
	}

	return false;
}

bool
ContainmentInterface::hasLauncher(ContainmentInterface::Target target, const KService::Ptr &service)
{
	if (target != TaskManager)
		return false;

	if (service && m_containment->pluginMetaData().pluginId() == QLatin1String("org.kde.panel")) {
		auto *taskManager = findTaskManagerApplet();
		if (!taskManager)
			return false;

		auto *taskManagerQuickItem = PlasmaQuick::AppletQuickItem::itemForApplet(taskManager);
		if (!taskManagerQuickItem)
			return false;

		bool ret;
		QMetaObject::invokeMethod(taskManagerQuickItem,
								  "hasLauncher",
								  Q_RETURN_ARG(bool, ret),
								  Q_ARG(QUrl, QUrl(QLatin1String("applications:") + service->storageId())));
		return ret;
	}

	return false;
}

void
ContainmentInterface::addLauncher(ContainmentInterface::Target target, const KService::Ptr &service)
{
	Plasma::Corona *corona = m_containment->corona();
	if (!corona)
		return;

	QUrl url = QUrl::fromLocalFile(service->entryPath());

	switch (target) {

	case Desktop: {
		Plasma::Containment *containment = corona->containmentForScreen(m_containment->screen(), QString(), QString());
        if (!containment)
            return;

		const QStringList &containmentProvides = containment->pluginMetaData().value(u"X-Plasma-Provides", QStringList());

		if (containmentProvides.contains(QLatin1String("org.kde.plasma.filemanagement"))) {
			auto *folderQuickItem = PlasmaQuick::AppletQuickItem::itemForApplet(containment);
			if (!folderQuickItem)
                return;

			QMetaObject::invokeMethod(folderQuickItem, "addLauncher", Q_ARG(QVariant, url));
		} else {
			containment->createApplet(QStringLiteral("org.kde.plasma.icon"), QVariantList() << url);
		}

		break;
    }
	case Panel: {
		if (m_containment->pluginMetaData().pluginId() == QLatin1String("org.kde.panel")) {
			m_containment->createApplet(QStringLiteral("org.kde.plasma.icon"), QVariantList() << url);
		}

		break;
	}
	case TaskManager: {
		if (m_containment->pluginMetaData().pluginId() == QLatin1String("org.kde.panel")) {
			auto *taskManager = findTaskManagerApplet();
			if (!taskManager)
				return;

			auto *taskManagerQuickItem = PlasmaQuick::AppletQuickItem::itemForApplet(taskManager);
            if (!taskManagerQuickItem)
                return;

            QMetaObject::invokeMethod(taskManagerQuickItem, "addLauncher", Q_ARG(QUrl, url));
		}

		break;
    }		
	}
}

Plasma::Applet
*ContainmentInterface::findTaskManagerApplet()
{
	const QList<Plasma::Applet *> applets = m_containment->applets();
	const auto found = std::ranges::find_if(applets, [this](const Plasma::Applet *applet) {
		return m_knownTaskManagers.contains(applet->pluginMetaData().pluginId());
	});
	return found != applets.cend() ? *found : nullptr;
}
