/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "iteminfo.h"

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
	Q_PROPERTY(bool active READ active NOTIFY activeChanged FINAL)
	// Whether or not the url is valid
	Q_PROPERTY(bool valid READ valid NOTIFY infoChanged FINAL)
	// The generic name of the applet
	Q_PROPERTY(QString genericName READ genericName NOTIFY infoChanged FINAL)
public:
	ServiceMenuApplet(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args);
	~ServiceMenuApplet() override;
	
	bool hasMenu() const { return !m_menu.isNull(); }
	bool active() const { return m_active; }
	bool valid() const { return m_info.isValid(); }
	QString genericName() const { return m_info.genericName(); }

	// Show/hide the menu in QML
	Q_INVOKABLE void toggleMenu();

	// Returns contextual actions for the specific applet
	QList<QAction *> contextualActions() override;

public Q_SLOTS:
	void init() override;

Q_SIGNALS:
	void hasMenuChanged();
	void activeChanged();
	void infoChanged();
	void configurationChanged();	

protected:
	// Getter and setters for the item info. Note: one of the setInfos *must* be called by the
	// subclasses at some point, as the menu *will not* be initialized if the info is invalid!
	ItemInfo info() const { return m_info; }
	void setInfo(const QUrl &url) { m_info = ItemInfo(url); Q_EMIT infoChanged(); }
	void setInfo(const KService::Ptr &service) { m_info = ItemInfo(service); Q_EMIT infoChanged(); }
	void setInfo(const KServiceGroup::Ptr &group) { m_info = ItemInfo(group); Q_EMIT infoChanged(); }	
							
protected Q_SLOTS:
	void initMenu();	
	
private:
	// Creates a given ServiceMenu object. Only ever called by the base class inside of initMenu()
	// Implement this in a subclass and return a pointer to a *parentless* ServiceMenu
	virtual ServiceMenu *createMenu() = 0;

	// Sets the toggled state of the applet
	void setActive(bool active) {
		if (m_active != active) {
			m_active = active;
			Q_EMIT activeChanged();
		}
	}

	// Deletes the currently loaded ServiceMenu class
	void deleteMenu();

	// Calculates the position at which the menu should be shown at.
	QPoint popupPosition();

	// Pointers
	QPointer<ServiceMenu> m_menu;

	// QList containing the extra actions we add to the applet's contextual actions
	QList<QAction *> m_actions;

	// Whether or not the menu is active
	bool m_active = false;

	// The item info of what this applet points to
	ItemInfo m_info;
};
