/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "panelmenu.h"

class RemoveMenu final : public PanelMenu
{
	/* Menu which gives the user the option to delete applets and icons from a given panel containment */
	Q_OBJECT
public:
	explicit RemoveMenu(Plasma::Containment *containment, QWidget *parent = nullptr);

protected:
	void initialize() override;

private:
	void populate() override;

	// Populates a given remove submenu with the applets contained in appletList
	static void populateSubMenu(QMenu *menu, const QList<QPointer<Plasma::Applet>> &appletList);

	// Removes all applets from a given list
	static void removeAllApplets(const QList<QPointer<Plasma::Applet>> &appletList);

	// Plasma::Applets which contain icons, and all other applets
	QList<QPointer<Plasma::Applet>> m_icons;
	QList<QPointer<Plasma::Applet>> m_applets;
};
