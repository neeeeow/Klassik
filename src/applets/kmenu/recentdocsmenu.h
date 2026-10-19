/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicemenu.h"

class RecentDocsMenu final : public ServiceMenu
{
	Q_OBJECT
	
public:
	explicit RecentDocsMenu(ServiceMenuApplet *applet, QWidget *parent = nullptr);
    RecentDocsMenu(const QString &title, ServiceMenuApplet *applet, QWidget *parent = nullptr);
	RecentDocsMenu(const QIcon &icon, const QString &title, ServiceMenuApplet *applet, QWidget *parent = nullptr);
	
private:
	void rebuild() override {}
	void populate() override;

private Q_SLOTS:
	void forgetDocuments() const;
};
