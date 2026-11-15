/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "panelmenu.h"
#include "iteminfo.h"

#include <KServiceGroup>

class AddAppsMenu final : public PanelMenu
{
	/*
	  Menu which displays the "Add Applications" menu on the panel, using a given
	  KServiceGroup.
	*/
	Q_OBJECT
public:
	explicit AddAppsMenu(Plasma::Applet *applet, QWidget *parent = nullptr);
    AddAppsMenu(const KServiceGroup::Ptr &group, Plasma::Applet *applet, QWidget *parent = nullptr);

protected:
	void initialize() override {}

	// Returns the ItemInfo of the menu
	ItemInfo info() const { return m_info; }

private:
	void populate() override;

	// Returns a list of QAction*s corresponding to the hierarchy of the menu's
	// KServiceGroup, all parented to this
	QList<QAction *> createActionsFromServiceGroup();
	
	// The ItemInfo of the group
	ItemInfo m_info;
};

