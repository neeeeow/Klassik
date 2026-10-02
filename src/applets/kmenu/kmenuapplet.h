/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QQuickItem>
#include <QPointer>

#include <KConfigLoader>

#include <Plasma/Applet>

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
		T getConfigValue(const QString &key) {
		KConfigLoader *configLoader = configScheme();
		if (!configLoader)
			return T();	

		QVariant value = configLoader->property(key);
		if (value.isNull() || !value.isValid()) {
			qWarning("KMenuApplet: Key not found!");
			return T();
		}
		if (!value.canConvert<T>()) {
			qWarning("KMenuApplet: Key type mismatch!");
			return T();
		}

		return value.value<T>();				
	}

Q_SIGNALS:
	void menuActiveChanged();

private:
	QPointer<ContainmentInterface> m_containmentInterface = nullptr;
	QPointer<KMenu> m_menu = nullptr;
	QTimer *m_reinitTimer = nullptr;

	QPoint popupPosition(QQuickItem *item);
};
