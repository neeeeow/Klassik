/*
  SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QMenu>
#include <QPointer>
#include <QUrl>

#include <KService>

namespace Plasma {
	class Applet;
	class Containment;
}
class ItemInfo;

class PanelMenu : public QMenu
{
	/*
	  Base class for menu which lives on a Plasma applet/containment. Actions are repopulated every
	  time the menu is shown. Useful for things like application menus.
	*/
	Q_OBJECT
public:
	explicit PanelMenu(Plasma::Applet *applet, QWidget *parent = nullptr);
	PanelMenu(const ItemInfo &info, Plasma::Applet *applet, QWidget *parent = nullptr);
	PanelMenu(const QString &title, Plasma::Applet *applet, QWidget *parent = nullptr);
	PanelMenu(const QIcon &icon, const QString &title, Plasma::Applet *applet, QWidget *parent = nullptr);

	//
	//
	// Static helper functions
	// These functions can be used with various panel menus and applets
	//
	//

	// Executes a given KService, with the option to open a specific
	// QUrl in the KService
	static void execService(const KService::Ptr &service, const QUrl &url = QUrl());

	// Executes a given QUrl
	static void execUrl(const QUrl &url);

	// Returns a non-applet specific list of contextual actions for a given ItemInfo
	static QList<QAction *> contextualActions(const ItemInfo &info, QObject *parent = nullptr);

	// QAction* which opens a given file in the file explorer
	static QAction* openInFileExplorerAction(const ItemInfo &info, QObject *parent = nullptr);

	// QAction* which invokes the KRunner with a given argument
	static QAction* kRunnerAction(QObject *parent = nullptr);
	static QAction* kRunnerAction(const QString &arg, QObject *parent = nullptr);

	// QAction* which launches the menu editor for a given KService or KServiceGroup
	// pointed to by the ItemInfo. If no ItemInfo is specified, we simply open the menu editor at the root.
	static QAction* menuEditorAction(QObject *parent = nullptr);
	static QAction* menuEditorAction(const ItemInfo &info, QObject *parent = nullptr);	

	// Appends a separator QAction to a given QList. Returns the QAction if added (and nullptr
	// if no separator was added!)
	static QAction* appendSeparatorActionToList(QList<QAction *> &actions, QObject *parent = nullptr);

	// Cleans up mnemonics from a string
	static QString escapeMnemonics(QString string);																										 

public Q_SLOTS:
	// Marks the menu to be reinitialized (e.g. after a config change). The menu is
	// reinitialized when the menu is re-opened.
	void reinitialize() { m_initialized = false; m_fresh = false; }

	// Refreshes the contents of the menu
	void refreshContents();

	// Forces a refresh of the contents, even if m_fresh was true
	void forceRefresh() { m_fresh = false; refreshContents(); }
	
protected:
	enum class AddToTarget {
		Desktop = 0,
		Panel
	};
	
	// Initializes the menu. Overriden by subclasses. Use this for loading in
	// configuration values, setting up models, etc. Do *not* populate the menu
	// from here!
	virtual void initialize() = 0;
	
	// Returns the initialization state of the menu
	bool initialized() const { return m_initialized; }

	// Returns whether or not the menu contents are fresh, and ready
	// to be displayed
	bool fresh() const { return m_fresh; }

	// Pointer to the Plasma::Applet in which the menu is contained
	Plasma::Applet *applet() const;

	// Checks to see if an item can be added to the panel or desktop
	bool mayAddItem(AddToTarget target) const;

	// Adds an icon pointing to a given QUrl to either the desktop or
	// panel. Will return true if the operation was successful!
	bool addLauncher(AddToTarget target, const QUrl &url) const;

	// QMenu event overrides
	void actionEvent(QActionEvent *e) override;
	void mousePressEvent(QMouseEvent *ev) override;
	void mouseMoveEvent(QMouseEvent *ev) override;
	void mouseReleaseEvent(QMouseEvent *ev) override;

	// 'No Entries' action
	QAction* noEntriesAction();

	template <typename T>
	T getConfigValue(const QString &key, const T &fallback = T()) const {
		const QVariant value = configValue(key);
		if (!value.isValid())
			return fallback;
		if constexpr (std::is_enum_v<T>) {
			// KConfigLoader stores Enum entries as int
			bool ok = false;
			const int i = value.toInt(&ok);
			if (!ok) {
				qWarning() << "key" << key << "type mismatch!";
				return fallback;
			}
			return static_cast<T>(i);
		} else {
			if (value.metaType() != QMetaType::fromType<T>()) {
				qWarning() << "key" << key << "type mismatch!";
				return fallback;
			}
			return value.value<T>();
		}
	}


private:	
	// Populates the menu
	virtual void populate() = 0;

	// Returns the desktop containment of the current activity.
	// Will return nullptr if no valid containment can be found
	Plasma::Containment *desktopContainment() const;

	// Returns the panel containment of the applet. Note, will only return a containment
	// if it's a Klassik Panel, otherwise it will return nullptr!
	Plasma::Containment *panelContainment() const;

	// Jump list actions for a given KService
	static QList<QAction *> jumpListActions(const KService::Ptr &service, QObject *parent = nullptr);

	// Actions which correspond to KServices for opening a specific file.
	// Also appends the header title.
	// Will return an empty list for a directory!
	static QList<QAction *> openWithActions(const ItemInfo &info, QObject *parent = nullptr);

	// Gets the config value
	QVariant configValue(const QString &key) const;
	
	// Pointer to the plasma applet
	QPointer<Plasma::Applet> m_applet;

	bool m_initialized = false;
	bool m_fresh = false;

	// Initial drag position. We use std::optional here since it only needs to contain
	// data whenever a drag operation is occuring.
	std::optional<QPointF> m_startPos;

private Q_SLOTS:
	// Creates and displays the context menu
	void showContextMenu(const QPoint &pos);
};
