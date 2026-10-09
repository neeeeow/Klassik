/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicemenu.h"

namespace KActivities::Stats {
    class ResultModel;
}

class RecentDocsMenu final : public ServiceMenu
{
	Q_OBJECT
	
public:
	explicit RecentDocsMenu(KMenuApplet *applet, QWidget *parent = nullptr);
	explicit RecentDocsMenu(const QString &title, KMenuApplet *applet, QWidget *parent = nullptr);
	
private:
	void populate() override;
	void refreshContents() override {updateRecentDocs();}
	void updateRecentDocs();

	KActivities::Stats::ResultModel *m_fileList = nullptr;
};
