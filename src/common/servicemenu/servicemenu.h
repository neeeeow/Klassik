/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QMenu>
#include <QPointer>
#include <QUrl>

#include <KService>

class ServiceMenuApplet;

class ServiceMenu : public QMenu
{
	/*
	  Base class for menu containing actions which launch services and URLs. Designed to be
	  used with a Plasma::Applet subclass ServiceMenuApplet. Actions are repopulated every
	  time the menu is shown. Useful for things like application menus.
	*/
	Q_OBJECT
	
public:
	explicit ServiceMenu(ServiceMenuApplet *applet, QWidget *parent = nullptr);
	ServiceMenu(const QString &title, ServiceMenuApplet *applet, QWidget *parent = nullptr);
	ServiceMenu(const QIcon &icon, const QString &title, ServiceMenuApplet *applet, QWidget *parent = nullptr);

	// Functions for creating QActions, from either a service, a url, parented to the menu
	QAction* createActionFromService(const KService::Ptr &service, const QUrl &url = QUrl());	
	QAction* createActionFromUrl(const QUrl &url);

public Q_SLOTS:
	// Marks the menu to be reinitialized (e.g. after a config change). The menu is
	// reinitialized when the menu is re-opened.
	void reinitialize() { m_initialized = false; }

	// Refreshes the contents of the menu
	void refreshContents();

protected:
	// Returns the initialization state of the menu
	bool initialized() const { return m_initialized; }
	
	// Loads the configuration settings for tooltips and generic names
	void loadConfig();
	
	// Returns a pointer to the Plasma applet attached to our menu
	ServiceMenuApplet* applet() const;

	// Cleans up mnemonics from a string
	static QString escapeMnemonics(QString string);

	// QMenu overrides
	void actionEvent(QActionEvent *e) override;
	void mousePressEvent(QMouseEvent *ev) override;
	void mouseMoveEvent(QMouseEvent *ev) override;
	void mouseReleaseEvent(QMouseEvent *ev) override;

private:
	// Initializes the menu's self population. Do not call this function on menus which are
	// static, otherwise their contents will be wiped when the menu is shown
	void initialize(); 
	
	// Rebuilds internal values/models/configs used by a menu. Never called outside
	// of initialize()
	virtual void rebuild() = 0;

	// Populates the menu contents
	virtual void populate() = 0;
	
	// Creates and displays the context menu
	void showContextMenu(const QPoint &pos) const;

	// Pointer to the plasma applet
	QPointer<ServiceMenuApplet> m_applet;

	// Initialization state
	bool m_initialized = false;

	// Whether or not the menu contents are fresh (i.e. do not need updating)
	bool m_fresh = false;

	// Whether or not to display generic names for applications
	bool m_displayGenericName = true;

	// Initial drag position. We use std::optional here since it only needs to contain
	// data whenever a drag operation is occuring.
	std::optional<QPointF> m_startPos;
};
