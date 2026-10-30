/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicemenu.h"

#include <QUrl>

class DirMenu final : public ServiceMenu
{
	/* Menu which holds the contents of a directory */
	Q_OBJECT
public:
    DirMenu(const QUrl &url, ServiceMenuApplet *applet, QWidget *parent = nullptr);

private:
	void rebuild() override {}
	void populate() override;

	// The url of the folder to display
	QUrl m_url;
};
