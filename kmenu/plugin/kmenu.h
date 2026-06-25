#pragma once

#include "servicemenu.h"
#include "popupmenutitle.h"

#include <QObject>
#include <QMenu>

#include <PlasmaActivities/Stats/ResultModel>

#include <sessionmanagement.h>

class KMenu : public ServiceMenu
{
	Q_OBJECT;
	
public:
	explicit KMenu(Plasma::Containment *containment, QWidget *parent = nullptr);
	~KMenu() override;

protected:
	bool eventFilter(QObject *object, QEvent *event) override;
	void changeEvent(QEvent *event) override;
	void paintEvent(QPaintEvent *e) override;

	void mousePressEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent *e) override;
	void mouseMoveEvent(QMouseEvent *e) override;

private:	
	// Session manager
	SessionManagement m_session;
	
	// Side pixmaps
	QPixmap m_sidePixmap;
	QPixmap m_sideTilePixmap;
	
	// Separator actions
	PopupMenuTitle *m_allAppsHeader;
	PopupMenuTitle *m_actionsHeader;

	// ResultModels for recent apps
    KActivities::Stats::ResultModel *m_recentApps;

	// Action lists
	QList<QAction *> m_recentActions; // List of actions linking to recent applications
	QList<QAction *> m_recentDocumentsActions; // List of actions linking to recent documents
	QList<QAction *> m_applicationActions; // List of actions in the root of the applications list

	// Other
	QLineEdit *m_searchLineEdit;
	
	void initialize();
	
	bool loadSidePixmap();
	QRect sideImageRect();
	void colorize(QImage &image);
	
	void createRecentMenuItems();
	void updateRecent();
	void createApplicationsItems();
	void updateSearchResults();
	void updateApplications();
	void createActionsItems();
	void createSystemItems();

	QMouseEvent* translateMouseEvent( QMouseEvent* e );
};
