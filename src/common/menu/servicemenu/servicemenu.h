/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "panelmenu.h"

class ServiceMenuApplet;

class ServiceMenu : public PanelMenu
{
	/*
	  Base class for menu containing actions which launch services and URLs. Designed to be
	  used with a Plasma::Applet subclass ServiceMenuApplet.
	*/
	Q_OBJECT
	
public:
	explicit ServiceMenu(ServiceMenuApplet *applet, QWidget *parent = nullptr);
	ServiceMenu(const QString &title, ServiceMenuApplet *applet, QWidget *parent = nullptr);
	ServiceMenu(const QIcon &icon, const QString &title, ServiceMenuApplet *applet, QWidget *parent = nullptr);

protected:
	// Call ServiceMenu::initialize() in any further overrides of initialize!
	void initialize() override;
	
	// Returns a pointer to the Plasma applet attached to our menu
	ServiceMenuApplet* applet() const;

	// Takes a KService (and an optional url pointing to a file to open) and
	// creates a QAction which launches the service, parented to this.
	QAction* createActionFromService(const KService::Ptr &service, const QUrl &url = QUrl());

	// QAction which launches a given url. Includes overloaded version for manually
	// specifying icon and title
	QAction* createActionFromUrl(const QUrl &url);
	QAction* createActionFromUrl(const QUrl &url, const QIcon &icon, const QString &title);

private: 
	// Whether or not to display generic names for applications
	bool m_displayGenericName = true;
};
