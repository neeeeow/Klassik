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

	inline bool isMenuActive() const { return m_menuActive; }
	inline ContainmentInterface* containmentInterface() const { return m_containmentInterface; }

	enum MenuLocation {
		Above,
		Below,
		Right,
		Left
	};

	Q_INVOKABLE MenuLocation preferredMenuLocation(QQuickItem *root, Plasma::Types::Location panelLocation);

	Q_INVOKABLE void toggleMenu(QQuickItem *button, QQuickItem *root, Plasma::Types::Location panelLocation) {
		if (m_menuActive)
			hideMenu();
		else
			showMenu(button, root, panelLocation);
	}

	template <typename T>
		T getConfigValue(const QString &key) {
		KConfigLoader *configLoader = configScheme();
		if (!configLoader)
			return T();	

		QVariant value = configLoader->property(key);
		if (value.isNull() || !value.isValid()) {
			qWarning("AppletConfig: Key not found!");
			return T();
		}
		if (!value.canConvert<T>()) {
			qWarning("AppletConfig: Key type mismatch!");
			return T();
		}

		return value.value<T>();				
	}

Q_SIGNALS:
	void menuActiveChanged();

private:
	ContainmentInterface *m_containmentInterface = nullptr;
	QPointer<KMenu> m_menu = nullptr; // m_menu should never be a dangling pointer, but just in case	
	bool m_menuActive = false;

	void showMenu(QQuickItem *button, QQuickItem *root, Plasma::Types::Location panelLocation);
	void hideMenu();
	QPoint adjustedMenuPosition(QQuickItem *button, QQuickItem *root, Plasma::Types::Location panelLocation);
};
