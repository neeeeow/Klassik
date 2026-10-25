/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "containmentinterface.h"

#include <Plasma/Applet>
#include <Plasma/Containment>
#include <Plasma/Corona>
#include <PlasmaQuick/AppletQuickItem>

ContainmentInterface::ContainmentInterface(Plasma::Applet *applet)
	: QObject(applet), m_applet(applet)
{
}

Plasma::Containment *
ContainmentInterface::containment() const
{
	return m_applet ? m_applet->containment() : nullptr;
}

bool
ContainmentInterface::mayAddLauncher(ContainmentInterface::Target target) const
{
	Plasma::Containment *appletContainment = containment();
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
	default: {
		break;
	}
	}

	return false;
}

void
ContainmentInterface::addLauncher(ContainmentInterface::Target target, const KService::Ptr &service) const
{
	Plasma::Containment *appletContainment = containment();
	if (!appletContainment || !service)
		return;
	
	Plasma::Corona *corona = appletContainment->corona();
	if (!corona)
		return;

	const QUrl url = QUrl::fromLocalFile(service->entryPath());

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
			// Use default plasma icon for the desktop.
			// TODO: write a custom desktop icon applet...
			desktopContainment->createApplet(QStringLiteral("org.kde.plasma.icon"), QVariantList() << url);
		}

		break;
	}
	case Panel: {
		if (appletContainment->pluginMetaData().pluginId() == QLatin1String("com.github.neeeeow.klassik.panel")) {
			appletContainment->createApplet(QStringLiteral("com.github.neeeeow.klassik.panel.icon"), QVariantList() << url);
		}

		break;
	}
	default: {
		break;
	}		
	}
}
