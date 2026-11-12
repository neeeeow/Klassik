/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicemenu.h"

#include <KServiceGroup>

class ServiceGroupMenu : public ServiceMenu
{
	/* Builds an application menu using a given relative path */
	Q_OBJECT
public:
	explicit ServiceGroupMenu(ServiceMenuApplet *applet, QWidget *parent = nullptr);	
	ServiceGroupMenu(const KServiceGroup::Ptr &group, ServiceMenuApplet *applet, QWidget *parent = nullptr);

Q_SIGNALS:
	void searchFilterChanged(const QString &filter);
	
protected:
	// Returns a list of QAction*s corresponding to the hierarchy of the menu's
	// KServiceGroup, all parented to this
	QList<QAction *> createActionsFromServiceGroup();

	// Returns the current search filter
	QString searchFilter() const { return m_searchFilter; }

protected Q_SLOTS:
	// Sets and resets the search filter for the menu
	void setSearchFilter(const QString &filter) {
		if (filter != m_searchFilter) {
			m_searchFilter = filter;
			Q_EMIT searchFilterChanged(m_searchFilter);
		}
	}
	void resetSearchFilter() {
		if (!m_searchFilter.isEmpty()) {
			m_searchFilter = QString();
			Q_EMIT searchFilterChanged(m_searchFilter);
		}
	}

private:
	void populate() override;

	// Returns a list of the actions to filter.
	virtual QList<QAction *> actionsToFilter() const { return actions(); }

	// Filters through each action in a given list of actions
	static void filterActions(const QList<QAction *> &actions, QStringView filter);

	// Returns true if the service passes the search filter (i.e. if
	// the service contains the search filter string)
	static bool filterService(const KService::Ptr &service, QStringView filter);

	// Returns true if the service group passes the search filter
	static bool filterServiceGroup(const KServiceGroup::Ptr	&group, QStringView filter);

	// The relpath of the KServiceGroup used to build the menu
	QString m_relPath;

	// The string used to filter 
	QString m_searchFilter;

private Q_SLOTS:
	// Applies the search filter to the menu items
	void applySearchFilter();
};
