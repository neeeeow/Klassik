#pragma once

#include "kmenu.h"

#include <QObject>

#include <QQuickItem>
#include <QtQml/qqml.h>

#include <Plasma/Containment>

class KMenuManager : public QObject
{
	/* This class is in charge of creating our actual K Menu object. It is not possible to
	   directly pass a subclassed QMenu to QML, so we do this as a workaround. */
	Q_OBJECT
	Q_PROPERTY(bool menuActive READ isMenuActive NOTIFY menuActiveChanged)

private:
	enum panelLocation { // this simply is a translation of QML's plasmoid.location
		Floating,
		Desktop,
		FullScreen,
		TopEdge,
		BottomEdge,
		LeftEdge,
		RightEdge
	};		
	
public:
	KMenuManager(QObject *parent = nullptr);
	~KMenuManager() override;

	bool isMenuActive() const {
		return m_menuActive;
	}
	
	Q_INVOKABLE void initialize(QObject *containmentObject);
	Q_INVOKABLE void toggleMenu(QQuickItem *button, QQuickItem *root, panelLocation panelLocation) {
		if (m_menuActive)
			hideMenu();
		else
			showMenu(button, root, panelLocation);
	}

Q_SIGNALS:
	void menuActiveChanged();
	
private:
	Plasma::Containment *m_containment = nullptr;
	KMenu *m_menu = nullptr; // Pointer to our actual KMenu object
	bool m_menuActive = false;

	void showMenu(QQuickItem *button, QQuickItem *root, panelLocation panelLocation);
	inline void hideMenu() { if (m_menu) m_menu->close(); }
	QPoint adjustedMenuPosition(QQuickItem *button, QQuickItem *root, panelLocation panelLocation);
};
