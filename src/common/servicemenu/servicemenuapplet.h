/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QPointer>
#include <Plasma/Applet>

class QQuickItem;
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
	void init() override;

	bool hasMenu() const { return m_menu; }
	bool menuActive() const;
	ContainmentInterface* containmentInterface() const;

	// Show/hide the menu in QML
	Q_INVOKABLE void toggleMenu();

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

Q_SIGNALS:
	void hasMenuChanged();
	void menuActiveChanged();

protected:
	void initMenu();
	void deleteMenu();
	
private:
	// Creates a given ServiceMenu object. Only ever called by the base class inside of initMenu()
	// Implement this in a subclass and return a pointer to a *parentless* ServiceMenu
	virtual ServiceMenu *createMenu() = 0;

	QVariant configValue(const QString &key) const;
	QPoint popupPosition();

	QPointer<ContainmentInterface> m_containmentInterface;
	QPointer<ServiceMenu> m_menu;

	// Single shot timer used to coalesce config changes 
	QTimer *m_timer = nullptr;
};
