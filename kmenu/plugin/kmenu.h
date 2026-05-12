#pragma once

#include "popupmenutitle.h"

#include <QObject>
#include <QMenu>
#include <QEvent>
#include <QAction>
#include <QLineEdit>

#include <PlasmaActivities/Stats/ResultModel>

#include <sessionmanagement.h>

class KMenu : public QMenu
{
	Q_OBJECT;
	
public:
	explicit KMenu(QWidget *parent = nullptr);
	~KMenu() override;

protected:
	bool eventFilter(QObject *, QEvent *) override;
	void changeEvent(QEvent *) override;
	void paintEvent(QPaintEvent *) override;

private:
	// Session manager
	SessionManagement m_session;
	
	// Side pixmaps
	QPixmap m_sidePixmap;
	QPixmap m_sideTilePixmap;
	
	// Separator actions
	PopupMenuTitle *m_allAppsHeader;
	PopupMenuTitle *m_actionsHeader;
	
    KActivities::Stats::ResultModel *m_recentApps;; // ResultModel storing recent applications
	QList<QAction *> m_recentActions; // List of actions linking to recent applications
	QList<QAction *> m_applicationActions; // List of actions in the root of the applications list
	
	QLineEdit *m_searchLineEdit;
	
	void initialize();
	bool loadSidePixmap();
	QRect sideImageRect();
	void colorize(QImage &);
	void createRecentMenuItems();
	void updateRecent();
	void createApplicationsItems();
	void updateSearchResults();
	void updateApplications();
	void cleanupActionList(QList<QAction *> &);
	void createActionsItems();
	void createSettingsMenu();
};
