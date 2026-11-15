/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QUrl>
#include <QString>

#include <KService>
#include <KServiceGroup>

class ItemInfo
{
	/*
	  Class which stores information about menu/panel items, generated
	  from a QUrl
	 */	
public:
	enum class Type {
		Invalid,
		File,
		Directory,
		Service,
		ServiceGroup
	};
	
	explicit ItemInfo(const QUrl &url);
	explicit ItemInfo(const KService::Ptr &service);
	explicit ItemInfo(const KServiceGroup::Ptr &serviceGroup);
	ItemInfo(const QUrl &url, Type type);
	ItemInfo() = default;

	// Refresh the information based on the stored QUrl
	void refresh();

	// Is the item info valid?
	bool isValid() const { return m_type != Type::Invalid; }

	// Returns the item type
	Type type() const { return m_type; }

	// Returns the item name
	QString name() const { return m_name; }

	// Returns the item's generic name
	QString genericName() const { return m_genericName; }

	// Returns a comment for an item, can be used for tooltips, for example.
	QString comment() const { return m_comment; }

	// Returns the mimetype
	QString mimetype() const { return m_mimetype; }
	
	// Returns the item's icon name
	QString icon() const { return m_icon; }

	// Returns the url of the item
	QUrl url() const { return m_url; }

	// Whether or not the file exists locally
	bool exists() const;

	// Returns the local path of the file
	QString localPath() const { return m_url.toLocalFile(); }

	// Returns the KService that the item points to. NB: this will return services that are noDisplay()!
	KService::Ptr service() const { return (m_type == Type::Service) ? m_service : KService::Ptr(); }

	// Returns the KServiceGroup the item points to. NB: this will return empty groups, if they are valid!
	KServiceGroup::Ptr group() const { return (m_type == Type::ServiceGroup) ? m_group : KServiceGroup::Ptr(); }   

private:
	Type m_type = Type::Invalid;
	QString m_name;
	QString m_genericName;
	QString m_comment;
	QString m_mimetype;
	QString m_icon;
	QUrl m_url;
	KService::Ptr m_service;
	KServiceGroup::Ptr m_group;

	// Resets every field, other than the backing QUrl
	void reset();

	// Sets the url to a given value. Returns true if url is valid.
	bool setUrl(const QUrl &url);

	// Populates the ItemInfo. Note, populateFromType and populateFromUrl
	// require the backing QUrl m_url to be set, otherwise the ItemInfo will be reset.
	void populateFromType(Type type);
	void populateFromUrl();
	void populateFromService(const KService::Ptr &service);
	void populateFromServiceGroup(const KServiceGroup::Ptr &group);

	// Returns a KService from a given QUrl which points to a .desktop file
	// If a non-null KService::Ptr is returned, it can be safely assumed to be valid
	// and from a local file!
	static KService::Ptr serviceFromUrl(const QUrl &url);

	// Returns a QUrl which points to a given KServiceGroup
	static QUrl urlFromServiceGroup(const KServiceGroup::Ptr &group);

	// Returns a KServiceGroup from a QUrl which points to a folder in the
	// applications:/// scheme, which corresponds to an application group.
	// Once again, if a non-null KServiceGroup::Ptr is returned, it can
	// safely assumed to be valid!
	static KServiceGroup::Ptr serviceGroupFromUrl(const QUrl &url);
};

Q_DECLARE_METATYPE(ItemInfo)
