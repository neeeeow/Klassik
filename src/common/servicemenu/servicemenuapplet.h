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
	// Whether or not the menu is active
	Q_PROPERTY(bool menuActive READ isMenuActive NOTIFY menuActiveChanged FINAL)	
public:
	ServiceMenuApplet(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args);
	~ServiceMenuApplet() override;
	void init() override;

	bool isMenuActive() const;
	ContainmentInterface* containmentInterface() const;

	// Show/hide the menu from QML, popping it up at the location of button
	Q_INVOKABLE void toggleMenu(QQuickItem *button);

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
	void menuActiveChanged();

private:
	QPointer<ContainmentInterface> m_containmentInterface = nullptr;
	QPointer<ServiceMenu> m_menu = nullptr;

	// Loads a given ServiceMenu. Only ever called by the base class in init().
	// Implement this in a subclass and return a pointer to a *parentless* ServiceMenu
	virtual ServiceMenu *createMenu() = 0;

	QVariant configValue(const QString &key) const;
	QPoint popupPosition(QQuickItem *item) const;	
};
