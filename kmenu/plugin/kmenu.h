#pragma once

#include "popupmenutitle.h"

#include <QObject>
#include <QMenu>
#include <QRect>
#include <QAction>

#include <PlasmaActivities/Stats/ResultModel>

class KMenu : public QMenu
{
	Q_OBJECT;
	
public:
	explicit KMenu(QWidget *parent = nullptr);
	~KMenu() override;

private:
	// Separator actions
	PopupMenuTitle *m_recentHeader;
	PopupMenuTitle *m_allAppsHeader;
	PopupMenuTitle *m_actionsHeader;
	
    KActivities::Stats::ResultModel *m_recentApps;; // ResultModel storing recent applications
	QList<QAction *> m_recentActions; // List of actions linking to recent applications

	void initialize();
	void createRecentMenuItems();
	void updateRecent();
};
