/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicegroupmenu.h"

class SessionManagement;

class KMenu final : public ServiceGroupMenu
{
	Q_OBJECT
	
public:
	explicit KMenu(ServiceMenuApplet *applet, QWidget *parent = nullptr);

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
		bool showMostUsedApps = true;
		int numMostUsedApps = 3;
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
	
	// List of applications in the menu, for use with the search bar
	QList<QPointer<QAction>> m_applicationActions;
	QList<QAction *> applicationActions() const {
		return QList<QAction *>(m_applicationActions.begin(), m_applicationActions.end());
	}

	void rebuild() override;
	void populate() override;
	
	// Sets the margin dependant on the side pixmap setting
	void setMargins();

	// Side pixmap related functions
	void loadSidePixmap();
	QRect sideImageRect() const;

	// Returns the actions corresponding to the most used applications section
	QList<QAction *> mostUsedAppsActions();

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
