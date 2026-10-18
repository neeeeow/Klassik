/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QMenu>
#include <QPointer>
#include <QUrl>

#include <KService>
#include <KServiceGroup>

class ServiceMenuApplet;

class ServiceMenu : public QMenu
{
	Q_OBJECT
	
public:
	explicit ServiceMenu(ServiceMenuApplet *applet, QWidget *parent = nullptr);
    ServiceMenu(const QString &title, ServiceMenuApplet *applet, QWidget *parent = nullptr);
	ServiceMenu(const QIcon &icon, const QString &title, ServiceMenuApplet *applet, QWidget *parent = nullptr);

	// Initializes the menu's self population. Do not call this function on menus which are
	// static, otherwise their contents will be wiped when the menu is shown
	void initialize();   

	// Functions for creating QActions, from either a service, a url, or an action which launches a
	// url in the file explorer. All QActions are parented to the menu
	QAction* createActionFromService(const KService::Ptr &service, const QUrl &url = QUrl());	
	QAction* createActionFromUrl(const QUrl &url);
	QAction* createFileExplorerActionFromUrl(const QUrl &url);
	QAction* createActionFromKCM(const QString &id, const QString &name, const QString &icon);
	QList<QAction *> createActionsFromServiceActions(const KService::Ptr &service);
	QList<QAction *> createActionsFromServiceGroup(const KServiceGroup::Ptr &group);

public Q_SLOTS:
	// Reinitializes the menu's self population. Usually called after a config
	// change.
	void reinitialize();   

	// Refreshes the contents of the menu
    void refreshContents() {
		if (!m_initialized) return;
		clear();
		populate();
	}

protected:	 
	// Returns a pointer to the Plasma applet attached to our menu
	ServiceMenuApplet* applet() const;

	// Launches the menu editor and KRunner
	static void runMenuEditor(QString arg = QString());
	static void invokeKRunner(const QString &arg = QString());

	// Cleans up mnemonics from a string
	static QString escapeMnemonics(QString string);

	// QMenu overrides
	void actionEvent(QActionEvent *e) override;
	void mousePressEvent(QMouseEvent *ev) override;
	void mouseMoveEvent(QMouseEvent *ev) override;
	void mouseReleaseEvent(QMouseEvent *ev) override;

private:
	bool m_initialized = false; // Self-population initialization state

	// Whether or not to display generic names for applications
	bool m_displayGenericName = true;

	// Pointer to the plasma applet
	QPointer<ServiceMenuApplet> m_applet = nullptr;

	// Initial drag position. We use std::optional here since it only needs to contain
	// data whenever a drag operation is occuring.
	std::optional<QPointF> m_startPos;

	// Loads the configuration settings for tooltips and generic names
	void loadConfig();

	// Populates the menu contents
	virtual void populate() {}

	// Rebuilds internal values/models/configs used by a menu. Never called outside
	// of initialize()
	virtual void rebuild() {}

	// Notifies KActivities that a QUrl has been accessed by the K Menu
	void notifyAccessed(const QUrl &url) const;
	
	// Creates and displays the context menu
	void showContextMenu(const QPoint &pos) const;
};
