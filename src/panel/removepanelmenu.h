/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "panelmenu.h"

class RemovePanelMenu final : public PanelMenu
{
	/* Menu which allows the user to remove other panels on the desktop */
	Q_OBJECT
public:
	explicit RemovePanelMenu(Plasma::Containment *containment, QWidget *parent = nullptr);

protected:
	void initialize() override;

private:
	void populate() override;

	// The panels on the desktop
	QList<QPointer<Plasma::Containment>> m_panels;

private Q_SLOTS:
	// Removes a given panel
	void removePanel(Plasma::Containment *panel) const;
};
