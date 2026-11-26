/*
  SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicemenu.h"
#include "iteminfo.h"

class DirMenu final : public ServiceMenu
{
	/* Menu which holds the contents of a directory */
	Q_OBJECT
public:
    DirMenu(const ItemInfo &info, Plasma::Applet *applet, QWidget *parent = nullptr);

private:
	void populate() override;

	// The ItemInfo of the folder we display
	ItemInfo m_info;
};
