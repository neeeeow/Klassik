/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "iconapplet.h"
#include "dirmenu.h"
#include "servicegroupmenu.h"

#include <KPluginFactory>
#include <KLocalizedString>
#include <KService>
#include <KServiceGroup>
#include <KProtocolManager>
#include <KFileItem>

IconApplet::IconApplet(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
	: ServiceMenuApplet(parent, data, args) {
	setHasConfigurationInterface(false);
}

void
IconApplet::init()
{
	ServiceMenuApplet::init();
	populate(appletUrl());
	connect(this, &IconApplet::urlChanged, this, &IconApplet::populate);
	connect(this, &IconApplet::urlChanged, this, &IconApplet::initMenu);
}

ServiceMenu *
IconApplet::createMenu()
{
	const QUrl url = appletUrl();	
	if (!url.isValid())
		return nullptr;

	if (const KServiceGroup::Ptr group = serviceGroupFromUrl(url))
		return new ServiceGroupMenu(group->relPath(), this);
	else if (url.isLocalFile() &&
			 KFileItem(url).isDir() &&
			 KProtocolManager::supportsListing(url))
		return new DirMenu(url, this);
	else
		return nullptr;
}

void
IconApplet::populate(const QUrl &url)
{
	QString name, icon, genericName;
	if (const KService::Ptr service = serviceFromUrl(url)) {
		name = service->name();
		icon = !service->icon().isEmpty() ? service->icon() : QStringLiteral("application-x-executable");
		genericName = service->genericName();
	} else if (const KServiceGroup::Ptr group = serviceGroupFromUrl(url)) {
		name = group->caption();
		icon = !group->icon().isEmpty() ? group->icon() : QStringLiteral("applications-other");
		genericName = i18n("Application Menu");
	} else if (url.isValid()) {
		const KFileItem item(url);
		name = item.text();
		if (name.isEmpty() || name == QLatin1Char('.'))
			name = url.fileName();
		if (name.isEmpty() || url.scheme().startsWith(QLatin1String("http")))
			name = url.host();
		if (name.isEmpty())
			name = url.path();
		icon = item.iconName();
		genericName = item.mimeComment();
	}

	setTitle(name);
	setIcon(!icon.isEmpty() ? icon : QStringLiteral("unknown"));
	setGenericName(genericName);
}

QUrl
IconApplet::appletUrl() const
{
	return config().readEntry(QStringLiteral("url"), QUrl());
}

void
IconApplet::setUrl(const QUrl &newUrl)
{
	if (appletUrl() != newUrl) {
		config().writeEntry(QStringLiteral("url"), newUrl);
		Q_EMIT urlChanged(newUrl);
		Q_EMIT configNeedsSaving();
	}
}

QString
IconApplet::genericName() const
{
	return m_genericName;
}

void
IconApplet::setGenericName(const QString &genericName)
{
	if (genericName != m_genericName) {
		m_genericName = genericName;
		Q_EMIT genericNameChanged(genericName);
	}
}

bool
IconApplet::isValid() const
{
	return appletUrl().isValid();
}

void
IconApplet::run()
{
	if (hasMenu())
		toggleMenu();
	else {	
		const QUrl url = appletUrl();
		if (!url.isValid())
			return;

		if (const KService::Ptr service = serviceFromUrl(url))
			execService(service);
		else
			execUrl(url);
	}
}

K_PLUGIN_CLASS_WITH_JSON(IconApplet, "metadata.json")

#include "iconapplet.moc"
