/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicemenu.h"

class ServiceGroupMenu final : public ServiceMenu
{
	/* Builds an application menu using a given KServiceGroup */
	Q_OBJECT
public:
	ServiceGroupMenu(const KServiceGroup::Ptr &group, ServiceMenuApplet *applet, QWidget *parent = nullptr);

	// Sets a new KServiceGroup to be used.
	void setServiceGroup(const KServiceGroup::Ptr &group);
private:
	// The KServiceGroup used to build the menu
	KServiceGroup::Ptr m_group;
	
	void rebuild() override;
	void populate() override;
};
