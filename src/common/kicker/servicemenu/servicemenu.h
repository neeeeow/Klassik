/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "panelmenu.h"

class ServiceMenu : public PanelMenu
{
	/*
	  Base class for menu containing actions which launch services and URLs. Designed to be
	  used with a Plasma::Applet subclass ServiceMenuApplet.
	*/
	Q_OBJECT
	
public:
	explicit ServiceMenu(Plasma::Applet *applet, QWidget *parent = nullptr);
	ServiceMenu(const ItemInfo &info, Plasma::Applet *applet, QWidget *parent = nullptr);
	ServiceMenu(const QString &title, Plasma::Applet *applet, QWidget *parent = nullptr);
	ServiceMenu(const QIcon &icon, const QString &title, Plasma::Applet *applet, QWidget *parent = nullptr);

protected:
	// NB: this *must* be called by subclasses if initialize is overriden, since we
	// set tooltip and generic name policy here!
	void initialize() override;
	
	// Creates an action from a given ItemInfo
    QAction* createActionFromItemInfo(const ItemInfo &info);

private: 
	// Whether or not to display generic names for applications
	bool m_showGenericName = false;
};
