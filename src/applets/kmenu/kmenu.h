/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicemenu.h"

class SessionManagement;
namespace KActivities::Stats {
    class ResultModel;
}

class KMenu final : public ServiceMenu
{
	Q_OBJECT
	
public:
	explicit KMenu(KMenuApplet *applet, QWidget *parent = nullptr);

	void initialize() override;
	void reinitialize() override;

protected:	
	void changeEvent(QEvent *event) override;
	void paintEvent(QPaintEvent *e) override;

	void mousePressEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent *e) override;
	void mouseMoveEvent(QMouseEvent *e) override;

private:	
	// Struct holding the config keys used in the KMenu class,
	// to avoid loading them many times unnecessarily.
	struct Config {
		bool drawSideImage = true;
		bool showTitles = true;
		bool showSearch = true;
		bool showRecentApps = true;
		int numRecentApps = 3;
		bool showRecentDocs = true;
		bool showSystem = true;
		bool showSettings = true;		
	};
	Config m_config;	
		
	// Session manager
	SessionManagement *m_session = nullptr;
	
	// Side pixmaps
	QPixmap m_sidePixmap;
	QPixmap m_sideTilePixmap;
	
	// Action to which we anchor the applications 
	QPointer<QAction> m_applicationsAnchor = nullptr;

	// ResultModels for recent apps
    KActivities::Stats::ResultModel *m_recentApps = nullptr;

	// Action lists
	QList<QAction *> m_recentActions; // List of actions linking to recent applications
	QList<QAction *> m_applicationActions; // List of actions in the root of the applications list
	QList<QAction *> m_sessionActions; // List of actions for the session

	// Flags which mark whether any of our action list actions are dirty
	bool m_recentDirty = false;
	bool m_applicationDirty = false;
	bool m_sessionDirty = false;

	void refreshContents() override;
	
	// Sets the margin dependant on the side pixmap setting
	void setMargins();

	// Side pixmap related functions
	void loadSidePixmap();
	QRect sideImageRect() const;

	// Functions for populating the menu
	void createRecentMenuItems();
	void updateRecent();
	void createApplicationsItems();   
	void updateApplications();
	void updateSessionActions();

	// Enables/disables actions based on a search filter. If the search filter text
	// is empty, all actions are enabled. Returns a bool corresponding to whether or
	// not any actions in the hierarchy are enabled.
	bool applySearchFilter(const QList<QAction *> &actions, QStringView text);

	// Translate mouse event, allowing actions to be highlighted if the
	// mouse is over the side image 
	std::unique_ptr<QMouseEvent> translateMouseEvent( QMouseEvent* e );

	// KDE 3 colorize function
	void colorize(QImage &image) const;
};
