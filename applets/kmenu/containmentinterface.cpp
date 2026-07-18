/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "containmentinterface.h"

#include <Plasma/Corona>
#include <PlasmaQuick/AppletQuickItem>

ContainmentInterface::ContainmentInterface(Plasma::Applet *applet)
	: QObject(applet), m_applet(applet)
{
}

bool
ContainmentInterface::mayAddLauncher(ContainmentInterface::Target target)
{
	Plasma::Containment *appletContainment = m_applet->containment();
	if (!appletContainment)
		return false;
	
	Plasma::Corona *corona = appletContainment->corona();
	if (!corona)
		return false;

	switch (target) {
	case Desktop: {		
		Plasma::Containment *desktopContainment = corona->containmentForScreen(appletContainment->screen(), QString(), QString());
		if (desktopContainment)
			return (desktopContainment->immutability() == Plasma::Types::Mutable);       

        break;
	}
	case Panel: {
		if (appletContainment->pluginMetaData().pluginId() == QLatin1String("org.kde.panel"))
			return (appletContainment->immutability() == Plasma::Types::Mutable);

		break;
    }
	case TaskManager: {
		if (appletContainment->pluginMetaData().pluginId() == QLatin1String("org.kde.panel")) {
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

	Plasma::Containment *containment = m_applet->containment();
	if (!containment)
		return false;

	if (service && containment->pluginMetaData().pluginId() == QLatin1String("org.kde.panel")) {
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
	Plasma::Containment *appletContainment = m_applet->containment();
	if (!appletContainment)
		return;
	
	Plasma::Corona *corona = appletContainment->corona();
	if (!corona)
		return;

	QUrl url = QUrl::fromLocalFile(service->entryPath());

	switch (target) {

	case Desktop: {
		Plasma::Containment *desktopContainment = corona->containmentForScreen(appletContainment->screen(), QString(), QString());
        if (!desktopContainment)
            return;

		const QStringList &containmentProvides = desktopContainment->pluginMetaData().value(u"X-Plasma-Provides", QStringList());

		if (containmentProvides.contains(QLatin1String("org.kde.plasma.filemanagement"))) {
			auto *folderQuickItem = PlasmaQuick::AppletQuickItem::itemForApplet(desktopContainment);
			if (!folderQuickItem)
                return;

			QMetaObject::invokeMethod(folderQuickItem, "addLauncher", Q_ARG(QVariant, url));
		} else {
			desktopContainment->createApplet(QStringLiteral("org.kde.plasma.icon"), QVariantList() << url);
		}

		break;
    }
	case Panel: {
		if (appletContainment->pluginMetaData().pluginId() == QLatin1String("org.kde.panel")) {
			appletContainment->createApplet(QStringLiteral("org.kde.plasma.icon"), QVariantList() << url);
		}

		break;
	}
	case TaskManager: {
		if (appletContainment->pluginMetaData().pluginId() == QLatin1String("org.kde.panel")) {
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
	Plasma::Containment *containment = m_applet->containment();
	if (!containment)
		return nullptr;
	
	const QList<Plasma::Applet *> applets = containment->applets();
	const auto found = std::ranges::find_if(applets, [this](const Plasma::Applet *applet) {
		return m_knownTaskManagers.contains(applet->pluginMetaData().pluginId());
	});
	return found != applets.cend() ? *found : nullptr;
}
