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

#include <PlasmaActivities/Stats/ResultModel>

class KMenuApplet;

class ServiceMenu : public QMenu
{
	Q_OBJECT
	
public:
	explicit ServiceMenu(KMenuApplet *applet, QWidget *parent = nullptr);
	explicit ServiceMenu(const QString &title, KMenuApplet *applet, QWidget *parent = nullptr);
	virtual ~ServiceMenu() = default;

	void cleanupActionList(QList<QAction *> &);

	// Functions for creating QActions, from either a service, a url, or an action which launches a
	// url in the file explorer. All QActions are parented to the menu
	QAction* createActionFromService(const KService::Ptr &, const QUrl &url = QUrl());
	QAction* createActionFromUrl(const QUrl &);
	QAction* createFileExplorerActionFromUrl(const QUrl &url, const QIcon &icon, const QString &title);

protected:
	// Initializes the menu (not called automatically!)
	// This must be called by any subclass, as it enables/disables tooltips
	// depending on the config
	virtual void initialize();
	
	// Reinitializes the menu (usually after a config change)
	// NOTE: it's a wise idea to reimplement this in a subclass
	// in case there any member variables, etc, you need to clear.
	// The base reinitialize() simply clears out the menu and calls
	// initialize() again.
	virtual void reinitialize();
	
	// Return the initialized flag
	bool initialized() const { return m_initialized; }

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

	// QMenu overrides
	void actionEvent(QActionEvent *e) override;
	void mousePressEvent(QMouseEvent *ev) override;
	void mouseMoveEvent(QMouseEvent *ev) override;

	// Template for connecting all of the required signals to a result model
	template <typename Slot>
	void connectResultModel(KActivities::Stats::ResultModel *model, Slot slot) {
		connect(model, &KActivities::Stats::ResultModel::dataChanged, this, slot);
		connect(model, &KActivities::Stats::ResultModel::modelReset, this, slot);
		connect(model, &KActivities::Stats::ResultModel::rowsInserted, this, slot);
		connect(model, &KActivities::Stats::ResultModel::rowsMoved, this, slot);
		connect(model, &KActivities::Stats::ResultModel::rowsRemoved, this, slot);
	}

private:
	bool m_initialized; // Initialization state

	// Pointer to the plasma applet
	QPointer<KMenuApplet> m_applet = nullptr;
	
	QPointF m_startPos{-1.0,-1.0}; // Initial drag position

	// Creates and displays the context menu
	void showContextMenu(const QPoint &pos) const;
};
