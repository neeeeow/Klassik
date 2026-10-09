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

class QAbstractItemModel;
class KMenuApplet;

class ServiceMenu : public QMenu
{
	Q_OBJECT
	
public:
	explicit ServiceMenu(KMenuApplet *applet, QWidget *parent = nullptr);
	explicit ServiceMenu(const QString &title, KMenuApplet *applet, QWidget *parent = nullptr);

	// Initializes the menu (not called automatically!)
	// This must be called by any subclass, as it enables/disables tooltips
	// depending on the config. Override populate() to add contents to the menu
	// during initialization
	void initialize();
	
	// Reinitializes the menu (usually after a config change)
	// NOTE: it's a wise idea to reimplement this in a subclass
	// in case there any member variables, etc, you need to clear.
	// The base reinitialize() simply clears out the menu and calls
	// initialize() again.
	virtual void reinitialize();

	// Manually refresh the menu contents if dirty. Used primarily for computing popup positions
	// which depend on the menu contents.
	void refreshIfDirty() {
		if (!initialized() || !m_dirty) return;
		m_dirty = false;
		refreshContents();
	}

	// Functions for creating QActions, from either a service, a url, or an action which launches a
	// url in the file explorer. All QActions are parented to the menu
	QAction* createActionFromService(const KService::Ptr &service, const QUrl &url = QUrl());	
	QAction* createActionFromUrl(const QUrl &url);
	QAction* createFileExplorerActionFromUrl(const QUrl &url);
	QAction* createActionFromKCM(const QString &id, const QString &name, const QString &icon);
	QList<QAction *> createActionsFromServiceActions(const KService::Ptr &service);
	QList<QAction *> createActionsFromServiceGroup(const KServiceGroup::Ptr &group);

protected:	 
	// Returns and sets the initialized flag (in general, this should only ever
	// be called from within initialize/reinitialize).
	bool initialized() const { return m_initialized; }
	void setInitialized(bool initialized) { m_initialized = initialized; }	

	// Mark the menu as dirty, and request contents to be refreshed.
	void markDirty() { m_dirty = true; }

	// Returns a pointer to the Plasma applet attached to our menu
	KMenuApplet* applet() const;

	// Takes a QList of QActions * and removes them from the menu
	void cleanupActionList(QList<QAction *> &);

	// Launches the menu editor and KRunner
	static void runMenuEditor(QString arg = QString());
	static void invokeKRunner(const QString &arg = QString());

	// Cleans up mnemonics from a string
	static QString escapeMnemonics(QString string);

	// Connects to a QAbstractItemModel, and marks the menu as dirty whenever the
	// model is updated. additionalFlags contains a QList of additional flags that
	// must be marked as dirty, to be used by various subclasses as necessary.
	void connectModel(QAbstractItemModel *model, const QList<bool *> &additionalFlags = {});

	// QMenu overrides
	void actionEvent(QActionEvent *e) override;
	void mousePressEvent(QMouseEvent *ev) override;
	void mouseMoveEvent(QMouseEvent *ev) override;
	void mouseReleaseEvent(QMouseEvent *ev) override;

private:
	bool m_initialized = false; // Initialization state

	// Whether or not contents need to be refreshed when the menu is about to be shown
	bool m_dirty = false;

	// Whether or not to display generic names for applications
	bool m_displayGenericName = true;

	// Pointer to the plasma applet
	QPointer<KMenuApplet> m_applet = nullptr;

	// Initial drag position. We use std::optional here since it only needs to contain
	// data whenever a drag operation is occuring.
	std::optional<QPointF> m_startPos;

	// Performs the initial population of the menu. Only ever called during initialize().
	// If you need to populate/repopulate the menu, call initialize/reinitialize.
	virtual void populate() {}

	// Refreshes contents before the menu is shown, if it is dirty.
	// NOTE: This is not the same as a full reinitialization, which will clear the
	// entire menu and initialize every element again. This function can be used to
	// reinitialize certain elements which need to be updated frequently (such as
	// recent files). This must only be called when the menu is about to be shown.
	// Perform the initial population in populate().
	virtual void refreshContents() {}

	// Notifies KActivities that a QUrl has been accessed by the K Menu
	void notifyAccessed(const QUrl &url) const;
	
	// Creates and displays the context menu
	void showContextMenu(const QPoint &pos) const;
};
