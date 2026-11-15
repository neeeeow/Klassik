/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "panelmenu.h"

class RemoveAppletMenu final : public PanelMenu
{
	/* Menu which gives the user the option to delete applets and icons from a given panel containment */
	Q_OBJECT
public:
	explicit RemoveAppletMenu(Plasma::Containment *containment, QWidget *parent = nullptr);

protected:
	void initialize() override;

private:
	void populate() override;

	// Populates a given remove submenu with the applets contained in appletList
	void populateSubMenu(QMenu *menu, const QList<QPointer<Plasma::Applet>> &appletList) const;

	// Removes all applets from a given list
    void removeAllApplets(const QList<QPointer<Plasma::Applet>> &appletList) const;

	// Plasma::Applets which contain icons, and all other applets
	QList<QPointer<Plasma::Applet>> m_icons;
	QList<QPointer<Plasma::Applet>> m_applets;
};
