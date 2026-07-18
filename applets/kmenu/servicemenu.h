/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "kmenuapplet.h"

#include <QObject>
#include <QMenu>
#include <QPointer>

#include <KService>
#include <KServiceGroup>

class ServiceMenu : public QMenu
{
	Q_OBJECT;
	
public:
	explicit ServiceMenu(KMenuApplet *applet, QWidget *parent = nullptr);
	explicit ServiceMenu(const QString &title, KMenuApplet *applet, QWidget *parent = nullptr);
	virtual ~ServiceMenu() = default;

	// Initializes the menu
	virtual void initialize();

	void cleanupActionList(QList<QAction *> &);

	// Functions for creating QActions, from either a service, a url, or an action which launches a
	// url in the file explorer. All QActions are parented to the menu
	QAction* createActionFromService(const KService::Ptr &, const QUrl &url = QUrl());
	QAction* createActionFromUrl(const QUrl &);
	QAction* createFileExplorerActionFromUrl(const QUrl &url, const QIcon &icon, const QString &title);

protected:
	// Reinitializes the menu (usually after a config change)
	virtual void reinitialize();
	
	// Return the initialized flag
	bool initialized();

	// Set the initialized flag
	void setInitialized(bool);

	// Returns a pointer to the Plasma applet attached to our menu
    KMenuApplet* applet() const { return m_applet; }

	// Creates a list of QAction pointers (with nested submenus) from a given
	// KServiceGroup, parented to the menu
	QList<QAction*> actionListFromServiceGroup(const KServiceGroup::Ptr &root);

	// Launches the menu editor and KRunner
	static void runMenuEditor(QString arg = QString());
	static void invokeKRunner(QString arg = QString());
	
	void mousePressEvent(QMouseEvent *ev) override;
	void mouseMoveEvent(QMouseEvent *ev) override;

private:
	bool m_initialized; // Initialization state

    KMenuApplet *m_applet = nullptr; // pointer to plasma applet
	
	QPointF m_startPos;

	void showContextMenu(const QPoint &pos);
};
