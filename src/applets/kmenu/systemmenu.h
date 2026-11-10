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
	explicit SystemMenu(ServiceMenuApplet *applet, QWidget *parent = nullptr);

protected:
	void initialize() override;

private:
	void populate() override;

	KFilePlacesModel *m_placesModel = nullptr;
};
