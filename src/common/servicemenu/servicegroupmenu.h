/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicemenu.h"

class ServiceGroupMenu : public ServiceMenu
{
	/* Builds an application menu using a given KServiceGroup */
	Q_OBJECT
public:
	explicit ServiceGroupMenu(ServiceMenuApplet *applet, QWidget *parent = nullptr);
	ServiceGroupMenu(const KServiceGroup::Ptr &group, ServiceMenuApplet *applet, QWidget *parent = nullptr);

	QString searchFilter() const { return m_searchFilter; }
	void setSearchFilter(const QString &filter) {
		if (filter != m_searchFilter) {
			m_searchFilter = filter;
			Q_EMIT searchFilterChanged(m_searchFilter);
		}
	}
	void resetSearchFilter() {
		m_searchFilter = QString();
		Q_EMIT searchFilterChanged(m_searchFilter);
	}

Q_SIGNALS:
	void searchFilterChanged(const QString &filter);
	
protected:
	// Returns a list of QAction*s corresponding to the hierarchy of the provided
	// KServiceGroup, all parented to this
	QList<QAction *> createActionsFromServiceGroup(const KServiceGroup::Ptr &group);

	// Enables/disables the action based on the search filter.
	void filterAction(QAction *action) const;

private:
	// The KServiceGroup used to build the menu
	KServiceGroup::Ptr m_group;

	// The string used to filter 
	QString m_searchFilter;

	void rebuild() override {}
	void populate() override;

	// Returns true if the service passes the search filter (i.e. if
	// the service does not contain the search filter string)
    static bool filterService(const KService::Ptr &service, QStringView filter);

	// Returns true if the service group passes the search filter
	static bool filterServiceGroup(const KServiceGroup::Ptr	&group, QStringView filter);

private Q_SLOTS:
	// Applies the search filter to the menu items
	virtual void applySearchFilter();
};
