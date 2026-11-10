/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QPointer>
#include <Plasma/Applet>

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
	
	bool hasMenu() const { return !m_menu.isNull(); }
	bool menuActive() const;	

	// Show/hide the menu in QML
	Q_INVOKABLE void toggleMenu();

	// Returns contextual actions for the specific applet
	QList<QAction *> contextualActions() override;

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

	// Pointers
	QPointer<ServiceMenu> m_menu;

	// Single shot timer used to coalesce config changes 
	QTimer *m_timer = nullptr;

	// QList containing the extra actions we add to the applet's contextual actions
	QList<QAction *> m_actions;
};
