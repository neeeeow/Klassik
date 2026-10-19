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

protected:
	// Returns a list of QAction*s corresponding to the hierarchy of the provided
	// KServiceGroup, all parented to this
	QList<QAction *> createActionsFromServiceGroup(const KServiceGroup::Ptr &group);

private:
	// The KServiceGroup used to build the menu
	KServiceGroup::Ptr m_group;

	void rebuild() override {}
	void populate() override;
};
