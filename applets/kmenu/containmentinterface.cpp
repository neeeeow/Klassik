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
	if (!m_applet)
		return false;
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
		if (appletContainment->pluginMetaData().pluginId() == QLatin1String("com.github.neeeeow.klassik.panel"))
			return (appletContainment->immutability() == Plasma::Types::Mutable);

		break;
    }
	case Quicklaunch: {
		if (appletContainment->pluginMetaData().pluginId() == QLatin1String("com.github.neeeeow.klassik.panel")) {
			auto *quicklaunch = findQuicklaunchApplet();
			if (!quicklaunch)
				return false;

			auto *quicklaunchQuickItem = PlasmaQuick::AppletQuickItem::itemForApplet(quicklaunch);
			if (!quicklaunchQuickItem)
				return false;

			return true;
		}
		
		break;
	}
	default: {
		break;
	}
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
		if (appletContainment->pluginMetaData().pluginId() == QLatin1String("com.github.neeeeow.klassik.panel")) {
			appletContainment->createApplet(QStringLiteral("org.kde.plasma.icon"), QVariantList() << url);
		}

		break;
	}
	case Quicklaunch: {
		if (appletContainment->pluginMetaData().pluginId() == QLatin1String("com.github.neeeeow.klassik.panel")) {
			auto *quicklaunch = findQuicklaunchApplet();
			if (!quicklaunch)
				return;

			auto *quicklaunchQuickItem = PlasmaQuick::AppletQuickItem::itemForApplet(quicklaunch);
			if (!quicklaunchQuickItem)
				return;

			QMetaObject::invokeMethod(quicklaunchQuickItem, "addLauncherUrl", Q_ARG(QVariant, QVariant(url.toString())));
		}
		
		break;
	}
	default: {
		break;
    }		
	}
}

Plasma::Applet
*ContainmentInterface::findQuicklaunchApplet()
{
	if (!m_applet)
		return nullptr;
	Plasma::Containment *containment = m_applet->containment();
	if (!containment)
		return nullptr;
	
	const QList<Plasma::Applet *> applets = containment->applets();
	const auto found = std::ranges::find_if(applets, [this](const Plasma::Applet *applet) {
	    return applet->pluginMetaData().pluginId() == QLatin1String("com.github.neeeeow.klassik.quicklaunch");
	});
	return found != applets.cend() ? *found : nullptr;
}
