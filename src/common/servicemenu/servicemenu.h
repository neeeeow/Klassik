/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicemenubase.h"

class ServiceMenu : public ServiceMenuBase
{
	Q_OBJECT
public:
	explicit ServiceMenu(ServiceMenuApplet *applet, QWidget *parent = nullptr);
    ServiceMenu(const QString &title, ServiceMenuApplet *applet, QWidget *parent = nullptr);
	ServiceMenu(const QIcon &icon, const QString &title, ServiceMenuApplet *applet, QWidget *parent = nullptr);

public Q_SLOTS:
	// Marks the menu to be reinitialized (e.g. after a config change). The menu is
	// reinitialized when the menu is re-opened.
	void reinitialize();   

	// Refreshes the contents of the menu
    void refreshContents();

private:
	// Initialization state
	bool m_initialized = false;

	// Initializes the menu's self population. Do not call this function on menus which are
	// static, otherwise their contents will be wiped when the menu is shown
	void initialize(); 
	
	// Rebuilds internal values/models/configs used by a menu. Never called outside
	// of initialize()
	virtual void rebuild() = 0;

	// Populates the menu contents
	virtual void populate() = 0;
};
