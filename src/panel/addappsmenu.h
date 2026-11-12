/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "panelmenu.h"

class AddAppsMenu final : public PanelMenu
{
	/*
	  Menu which displays the "Add Applications" menu on the panel, using a given
	  KServiceGroup relPath.
	*/
	Q_OBJECT
public:
	explicit AddAppsMenu(Plasma::Applet *applet, QWidget *parent = nullptr);
    AddAppsMenu(const QString &relPath, Plasma::Applet *applet, QWidget *parent = nullptr);

protected:
	void initialize() override {}

private:
	void populate() override;

	// Returns a list of QAction*s corresponding to the hierarchy of the menu's
	// KServiceGroup, all parented to this
	QList<QAction *> createActionsFromServiceGroup();
	
	QString m_relPath;
};

