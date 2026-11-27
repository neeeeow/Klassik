/*
  SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "iteminfo.h"

#include <QFileInfo>

#include <KFileItem>
#include <KLocalizedString>
#include <KDesktopFile>
#include <KProtocolManager>

using namespace Qt::StringLiterals;

ItemInfo::ItemInfo(const QUrl &url)
{
	// Set the internal url
    if (setUrl(url)) {
		// Then populate the rest
		if (auto service = serviceFromUrl(url)) {
			populateFromService(service);
		} else if (auto group = serviceGroupFromUrl(url))
			populateFromServiceGroup(group);
		else
			populateFromUrl();
	}
}

ItemInfo::ItemInfo(const KService::Ptr &service)
{
	if (service && setUrl(QUrl::fromLocalFile(service->entryPath())))
		populateFromService(service);
}

ItemInfo::ItemInfo(const KServiceGroup::Ptr &group)
{
	if (group && setUrl(urlFromServiceGroup(group)))
		populateFromServiceGroup(group);
}

ItemInfo::ItemInfo(const QUrl &url, Type type)
{
	if (setUrl(url))	
		populateFromType(type);
}

void
ItemInfo::refresh()
{
	ItemInfo::Type reqType = m_type;
	reset();
	populateFromType(reqType);
}

void
ItemInfo::reset()
{
	m_type = Type::Invalid;
	m_name = QString();
	m_genericName = QString();
	m_comment = QString();
	m_mimetype = QString();
	m_icon = QString();
	m_service = KService::Ptr();
	m_group = KServiceGroup::Ptr();
}

bool
ItemInfo::setUrl(const QUrl &url)
{
	if (!url.isValid() || url.scheme().isEmpty()) {
		m_url.clear();
		reset();
		return false;
	}
	m_url = url;
	return true;
}

bool
ItemInfo::exists() const
{
	return m_url.isLocalFile() && QFileInfo::exists(m_url.toLocalFile());
}

void
ItemInfo::populateFromType(Type type)
{
	switch (type) {
	case Type::File:
	case Type::Directory: {
		populateFromUrl();
		break;
	}
	case Type::Service: {
		if (auto service = serviceFromUrl(m_url))
			populateFromService(service);
		break;
	}
	case Type::ServiceGroup: {
		if (auto group = serviceGroupFromUrl(m_url))
			populateFromServiceGroup(group);
		break;
	}
	case Type::Invalid: {
		m_type = Type::Invalid;
		return;
	}
	}

	if (type != m_type)
		m_type = Type::Invalid;
}

void
ItemInfo::populateFromUrl()
{
	if (!m_url.isValid() || m_url.scheme().isEmpty()) {
		reset();
		return;
	}

	const bool isDir = m_url.isLocalFile() && QFileInfo(m_url.toLocalFile()).isDir();
	const KFileItem item = isDir
		? KFileItem(m_url, QStringLiteral("inode/directory"), S_IFDIR)
		: KFileItem(m_url, KFileItem::SkipMimeTypeFromContent);

	QString name = item.text();
	if (name.isEmpty() || name == QLatin1Char('.'))
		name = m_url.fileName();
	if (name.isEmpty() || m_url.scheme().startsWith("http"_L1))
		name = m_url.host();
	if (name.isEmpty())
		name = m_url.path();
	
	m_name = name;
	m_genericName = item.mimeComment();
	m_comment = m_url.toDisplayString(QUrl::PreferLocalFile);
	m_mimetype = item.mimetype();
	m_icon = !item.iconName().isEmpty() ? item.iconName() : QStringLiteral("unknown");
	m_type = (isDir && KProtocolManager::supportsListing(m_url)) ? Type::Directory : Type::File;

	m_service = KService::Ptr();
	m_group = KServiceGroup::Ptr();
}

void
ItemInfo::populateFromService(const KService::Ptr &service)
{
	if (!service || !service->isValid()) {
		reset();
		return;
	}

	m_type = Type::Service;	
	m_name = service->name();
	m_genericName = service->genericName();
	m_comment = service->comment();
	m_mimetype = QStringLiteral("application/x-desktop");
	m_icon = !service->icon().isEmpty() ? service->icon() : QStringLiteral("application-x-executable");
	m_service = service;
	m_group = KServiceGroup::Ptr();
}

void
ItemInfo::populateFromServiceGroup(const KServiceGroup::Ptr &group)
{
	if (!group || !group->isValid()) {
		reset();
		return;
	}

	m_type = Type::ServiceGroup;
	m_genericName = i18n("Application Menu");
	m_comment = i18np("%1 application", "%1 applications", group->childCount());
	m_mimetype = QStringLiteral("inode/directory"); // We want servicegroups to appear as directories
	if (group->relPath() == QLatin1Char('/')) {
		// If the relpath is '/', we have the root KServiceGroup, in which case we assign
		// a name and icon ourselves!
		m_name = i18n("Applications");
		m_icon = QStringLiteral("applications-all");
	} else {
		m_name = group->caption();
		m_icon = !group->icon().isEmpty() ? group->icon() : QStringLiteral("applications-other");
	}
	m_service = KService::Ptr();
	m_group = group;
}

KService::Ptr
ItemInfo::serviceFromUrl(const QUrl &url)
{
	if (!url.isValid() || !url.isLocalFile())
		return KService::Ptr();

	// First try to retrieve the service directly from the desktop path...
    KService::Ptr service = KService::serviceByDesktopPath(url.toLocalFile());
	if (service && service->isValid() && service->isApplication())
		return service;

	// ... if that fails, then we inspect the .desktop file and construct
	// a KService ourselves.
	const QString path = url.toLocalFile();
	if (!KDesktopFile::isDesktopFile(path) || !KDesktopFile::isAuthorizedDesktopFile(path))
		return KService::Ptr();

    service = KService::Ptr(new KService(path));
	if (!service || !service->isValid() || !service->isApplication())
		return KService::Ptr();
	
	return service;
}

QUrl
ItemInfo::urlFromServiceGroup(const KServiceGroup::Ptr &group)
{
	if (!group || !group->isValid())
		return QUrl();

	const QString relPath = group->relPath();
	QUrl url;
	url.setScheme("applications"_L1);
    url.setPath(relPath.startsWith(QLatin1Char('/')) ? relPath : QLatin1Char('/') + relPath); // Path must always begin with /
	return url.adjusted(QUrl::StripTrailingSlash); // NB: StripTrailingSlash *will not* strip a lone /, so if the path is / it will remain /
}

KServiceGroup::Ptr
ItemInfo::serviceGroupFromUrl(const QUrl &url)
{
	if (!url.isValid() || url.scheme() != "applications"_L1
		|| !url.path().startsWith(QLatin1Char('/')))
		return KServiceGroup::Ptr();

	// Strip the leading '/'
	QString relPath = url.path().mid(1);
	  
	// If the relPath doesn't have a trailing slash, we append one
	if (!relPath.endsWith(QLatin1Char('/')))
		relPath.append(QLatin1Char('/'));
	
	const KServiceGroup::Ptr group = KServiceGroup::group(relPath);
	if (!group || !group->isValid())
		return KServiceGroup::Ptr();
	
	return group;
}
