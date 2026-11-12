/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicemenu.h"

#include <KFileItem>

class DirMenu final : public ServiceMenu
{
	/* Menu which holds the contents of a directory */
	Q_OBJECT
public:
    DirMenu(const KFileItem &dir, ServiceMenuApplet *applet, QWidget *parent = nullptr);

private:
	void populate() override;

	// The KFileItem of the folder we display
	KFileItem m_dir;
};
