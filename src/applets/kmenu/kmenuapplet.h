/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QPointer>
#include <Plasma/Applet>

class QQuickItem;
class ContainmentInterface;
class KMenu;

class KMenuApplet : public Plasma::Applet
{
	Q_OBJECT
	Q_PROPERTY(bool menuActive READ isMenuActive NOTIFY menuActiveChanged)
	
public:
	KMenuApplet(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args);
	~KMenuApplet() override;
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
	QPointer<KMenu> m_menu = nullptr;

	QVariant configValue(const QString &key) const;
	QPoint popupPosition(QQuickItem *item);	
};
