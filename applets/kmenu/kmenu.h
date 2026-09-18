/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicemenu.h"
#include "popupmenutitle.h"

#include <sessionmanagement.h>

class KMenu : public ServiceMenu
{
	Q_OBJECT
	
public:
	explicit KMenu(KMenuApplet *applet, QWidget *parent = nullptr);
	~KMenu() override = default;

	// Resets the menu
	void reinitialize() override;

protected:
	// Initializes the menu (automatically done during construction)
	void initialize() override;
	
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
		bool showSettings = true;
		bool showRecentDocs = true;
	};
	Config m_config;	
		
	// Session manager
	SessionManagement m_session;
	
	// Side pixmaps
	QPixmap m_sidePixmap;
	QPixmap m_sideTilePixmap;
	
	// Action to which we anchor the applications 
	QAction *m_applicationsAnchor = nullptr;

	// ResultModels for recent apps
    KActivities::Stats::ResultModel *m_recentApps = nullptr;

	// Action lists
	QList<QAction *> m_recentActions; // List of actions linking to recent applications
	QList<QAction *> m_applicationActions; // List of actions in the root of the applications list
	
	// Sets the margin dependant on the side pixmap setting
	void setMargins();

	// Side pixmap related functions
	void loadSidePixmap();
	QRect sideImageRect() const;

	// Functions for populating the menu
	void createRecentMenuItems();
	void updateRecent();
	void createApplicationsItems();
	void updateSearchResults();
	void updateApplications();

	// Translate mouse event, allowing actions to be highlighted if the
	// mouse is over the side image 
	std::unique_ptr<QMouseEvent> translateMouseEvent( QMouseEvent* e );

	// KDE 3 colorize function
	void colorize(QImage &image) const;
};
