/*
  SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "panelmenu.h"
#include "iteminfo.h"

class AddAppsMenu final : public PanelMenu
{
	/*
	  Menu which displays the "Add Applications" menu on the panel, using a given
	  KServiceGroup.
	*/
	Q_OBJECT
public:
	explicit AddAppsMenu(Plasma::Applet *applet, QWidget *parent = nullptr);
    AddAppsMenu(const ItemInfo &info, Plasma::Applet *applet, QWidget *parent = nullptr);

protected:
	void initialize() override {}

private:
	void populate() override;

	// Returns a list of QAction*s corresponding to the hierarchy of the menu's
	// KServiceGroup, all parented to this
	QList<QAction *> createActionsFromServiceGroup();
	
	// The ItemInfo of the group
	ItemInfo m_info;
};

