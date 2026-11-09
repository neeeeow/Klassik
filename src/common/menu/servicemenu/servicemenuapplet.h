/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QPointer>
#include <KService>
#include <KServiceGroup>
#include <Plasma/Applet>

class ContainmentInterface;
class ServiceMenu;

class ServiceMenuApplet : public Plasma::Applet
{
	/*
	  Template Plasma::Applet subclass for creating applets which popup
	  a ServiceMenu (or one of its subclasses!)
	*/
	Q_OBJECT
	// Whether or not the menu has been loaded
	Q_PROPERTY(bool hasMenu READ hasMenu NOTIFY hasMenuChanged FINAL)
	// Whether or not the menu is active
	Q_PROPERTY(bool menuActive READ menuActive NOTIFY menuActiveChanged FINAL)	
public:
	ServiceMenuApplet(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args);
	~ServiceMenuApplet() override;

	ContainmentInterface* containmentInterface() const;
	
	bool hasMenu() const { return !m_menu.isNull(); }
	bool menuActive() const;	

	// Show/hide the menu in QML
	Q_INVOKABLE void toggleMenu();

	// Returns a KService from a given QUrl which points to a .desktop file
	// If a non-null KService::Ptr is returned, it can be safely assumed to be valid
	// and from a local file!
	static KService::Ptr serviceFromUrl(const QUrl &url);

	// Returns a QUrl which points to a given KServiceGroup
	static QUrl urlFromServiceGroup(const KServiceGroup::Ptr &group);

	// Returns a KServiceGroup from a QUrl which points to a folder in the
	// applications:/// scheme, which corresponds to an application group.
	// Once again, if a non-null KServiceGroup::Ptr is returned, it can
	// safely assumed to be valid!
	static KServiceGroup::Ptr serviceGroupFromUrl(const QUrl &url);

	// Executes a given KService, with the option to open a specific
	// QUrl in the KService
	static void execService(const KService::Ptr &service, const QUrl &url = QUrl());

	// Executes a given QUrl
	static void execUrl(const QUrl &url);

	// Returns contextual actions for the specific applet
	QList<QAction *> contextualActions() override;
	// Returns a non-applet specific list of contextual actions for a given QUrl.
	// Used within both the applet's contextual actions (the function above) and in
	// the contextual actions of menus.
	static QList<QAction *> contextualActions(const QUrl &url, QObject *parent = nullptr);

	// QAction* which opens the file pointed to by the QUrl
	// in the file explorer.
	static QAction* openInFileExplorerAction(const QUrl &url, QObject *parent = nullptr);

	// QAction* which invokes the KRunner with a given argument
	static QAction* kRunnerAction(const QString &arg = QString(), QObject *parent = nullptr);

	// QAction* which launches the menu editor for a given KService or KServiceGroup
	// pointed to by the QUrl. If the QUrl is empty, we simply open the menu editor at the root.
	static QAction* menuEditorAction(const QUrl &url = QUrl(), QObject *parent = nullptr);	

	// Cleans up mnemonics from a string
	static QString escapeMnemonics(QString string);

	// Config getter
	template <typename T>
	T getConfigValue(const QString &key) const {
		const QVariant value = configValue(key);
		if (!value.canConvert<T>()) {
			qWarning() << "key" << key << "type mismatch!";
			return T();
		}
		return value.value<T>();
	}

public Q_SLOTS:
	void init() override;

Q_SIGNALS:
	void hasMenuChanged();
	void menuActiveChanged();
							
protected Q_SLOTS:
	void initMenu();	
	
private:
	// Creates a given ServiceMenu object. Only ever called by the base class inside of initMenu()
	// Implement this in a subclass and return a pointer to a *parentless* ServiceMenu
	virtual ServiceMenu *createMenu() = 0;

	// Returns the QUrl that the applet points to. Used for generating contextual actions.
	// This does not need to be overriden and set - a blank QUrl() simply means no additional
	// contextual actions will be generated for the applet
	virtual QUrl appletUrl() const { return QUrl(); }

	// Gets the config value
	QVariant configValue(const QString &key) const;

	// Deletes the currently loaded ServiceMenu class
	void deleteMenu();

	// Calculates the position at which the menu should be shown at.
	QPoint popupPosition();

	// Jump list actions for a given KService
	static QList<QAction *> jumpListActions(const KService::Ptr &service, QObject *parent = nullptr);

	// Actions which correspond to KServices for opening a specific file.
	// Also appends the header title.
	// Will return an empty list for a directory!
	static QList<QAction *> openWithActions(const QUrl &url, QObject *parent = nullptr);

	// Appends a separator QAction to a given QList
    static void appendSeparatorActionToList(QList<QAction *> &actions, QObject *parent = nullptr);

	// Pointers
	QPointer<ContainmentInterface> m_containmentInterface;
	QPointer<ServiceMenu> m_menu;

	// Single shot timer used to coalesce config changes 
	QTimer *m_timer = nullptr;

	// QList containing the extra actions we add to the applet's contextual actions
	QList<QAction *> m_actions;
};
