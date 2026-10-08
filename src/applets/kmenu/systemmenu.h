/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicemenu.h"

class KFilePlacesModel;

class SystemMenu final : public ServiceMenu
{
	Q_OBJECT
	
public:
	explicit SystemMenu(KMenuApplet *applet, QWidget *parent = nullptr);
	explicit SystemMenu(const QString &title, KMenuApplet *applet, QWidget *parent = nullptr);

	void initialize() override;

private:
	KFilePlacesModel *m_placesModel = nullptr;

	// Populates the menu
	void populate();
};
