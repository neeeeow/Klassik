#pragma once

#include "popupmenu.h"
#include "popupmenutitle.h"

#include <QObject>
#include <QMenu>

#include <PlasmaActivities/Stats/ResultModel>

#include <sessionmanagement.h>

class KMenu : public PopupMenu
{
	Q_OBJECT;
	
public:
	explicit KMenu(QWidget *parent = nullptr);
	~KMenu() override;

protected:
	bool eventFilter(QObject *, QEvent *) override;
	void changeEvent(QEvent *) override;
	void paintEvent(QPaintEvent *) override;

	void mousePressEvent(QMouseEvent *) override;
	void mouseReleaseEvent(QMouseEvent *) override;
	void mouseMoveEvent(QMouseEvent *) override;

private:
	// Session manager
	SessionManagement m_session;
	
	// Side pixmaps
	QPixmap m_sidePixmap;
	QPixmap m_sideTilePixmap;
	
	// Separator actions
	PopupMenuTitle *m_allAppsHeader;
	PopupMenuTitle *m_actionsHeader;

	// ResultModels for recent apps/documents
    KActivities::Stats::ResultModel *m_recentApps;
	KActivities::Stats::ResultModel *m_recentDocuments;

	// Action lists
	QList<QAction *> m_recentActions; // List of actions linking to recent applications
	QList<QAction *> m_recentDocumentsActions; // List of actions linking to recent documents
	QList<QAction *> m_applicationActions; // List of actions in the root of the applications list

	// Other
	PopupMenu *m_recentDocumentsMenu;	
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
	void createActionsItems();
	void createRecentDocumentsItems();
	void updateRecentDocuments();

	QMouseEvent* translateMouseEvent( QMouseEvent* e );
};
