#pragma once

#include "servicemenu.h"

#include <QObject>

#include <PlasmaActivities/Stats/ResultModel>

class RecentDocsMenu : public ServiceMenu
{
	Q_OBJECT;
	
public:
	explicit RecentDocsMenu(Plasma::Containment *containment, QWidget *parent = nullptr);
	explicit RecentDocsMenu(const QString &title, Plasma::Containment *containment, QWidget *parent = nullptr);
	~RecentDocsMenu() override;

private:
	void initialize();
	void updateRecentDocs();

	KActivities::Stats::ResultModel *m_fileList;
};
