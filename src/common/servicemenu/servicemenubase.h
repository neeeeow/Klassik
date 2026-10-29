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

class ServiceMenuBase : public QMenu
{
	/*
	  Base class containing actions which launch various services and URLs.
	  The base class is static, meaning that items are not dynamically refreshed.
	  Only use the base class to construct menus that are either short-lived (e.g. context menus)
	  or have items which are not expected to change in the long run. In cases where you need to
	  have items refresh on every popup, subclass ServiceMenu.
	*/
	Q_OBJECT
	
public:
	explicit ServiceMenuBase(ServiceMenuApplet *applet, QWidget *parent = nullptr);
    ServiceMenuBase(const QString &title, ServiceMenuApplet *applet, QWidget *parent = nullptr);
	ServiceMenuBase(const QIcon &icon, const QString &title, ServiceMenuApplet *applet, QWidget *parent = nullptr);  

	// Functions for creating QActions, from either a service, a url, or an action which launches a
	// url in the file explorer. All QActions are parented to the menu
	QAction* createActionFromService(const KService::Ptr &service, const QUrl &url = QUrl());	
	QAction* createActionFromUrl(const QUrl &url);
	QAction* createFileExplorerActionFromUrl(const QUrl &url);
	QAction* createActionFromKCM(const QString &id, const QString &name, const QString &icon);
	QList<QAction *> createActionsFromServiceActions(const KService::Ptr &service);	

protected:
	// Loads the configuration settings for tooltips and generic names
	void loadConfig();
	
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
	// Notifies KActivities that a QUrl has been accessed by the K Menu
	void notifyAccessed(const QUrl &url) const;
	
	// Creates and displays the context menu
	void showContextMenu(const QPoint &pos) const;

	// Whether or not to display generic names for applications
	bool m_displayGenericName = true;

	// Pointer to the plasma applet
	QPointer<ServiceMenuApplet> m_applet;

	// Initial drag position. We use std::optional here since it only needs to contain
	// data whenever a drag operation is occuring.
	std::optional<QPointF> m_startPos;
};
