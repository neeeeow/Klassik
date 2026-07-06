#pragma once

#include "servicemenu.h"

#include <QObject>

#include <PlasmaActivities/Stats/ResultModel>

class RecentDocsMenu : public ServiceMenu
{
	Q_OBJECT;
	
public:
	explicit RecentDocsMenu(KMenuApplet *applet, QWidget *parent = nullptr);
	explicit RecentDocsMenu(const QString &title, KMenuApplet *applet, QWidget *parent = nullptr);
	~RecentDocsMenu() override = default;

private:
	void initialize() override;
	void updateRecentDocs();

	KActivities::Stats::ResultModel *m_fileList = nullptr;
};
