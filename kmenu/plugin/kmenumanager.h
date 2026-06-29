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
	Q_OBJECT;

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
	
	Q_INVOKABLE void initialize(QObject *containmentObject);
	Q_INVOKABLE void showMenu(QQuickItem *button, QQuickItem *root, panelLocation panelLocation);

private:
	Plasma::Containment *m_containment;
	KMenu *m_menu = nullptr; // Pointer to our actual KMenu object
	
	QPoint adjustedMenuPosition(QQuickItem *button, QQuickItem *root, panelLocation panelLocation);
};
