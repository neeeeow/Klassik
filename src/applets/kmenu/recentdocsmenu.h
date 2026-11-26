/*
  SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicemenu.h"

class RecentDocsMenu final : public ServiceMenu
{
	Q_OBJECT
	
public:
	explicit RecentDocsMenu(Plasma::Applet *applet, QWidget *parent = nullptr);

protected:
	void initialize() override;
	
private:
	void populate() override;

	int m_numRecentDocs = 15;

private Q_SLOTS:
	void forgetDocuments() const;
};
