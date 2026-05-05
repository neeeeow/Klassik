#pragma once

#include "kmenu.h"

#include <QObject>

#include <QQuickItem>
#include <QtQml/qqml.h>

class KMenuManager : public QObject
{
	/* This class is in charge of creating our actual K Menu object. It is not possible to
	   directly pass a subclassed QMenu to QML, so we do this as a workaround. */
	Q_OBJECT;
	
public:
	explicit KMenuManager(QObject *parent = nullptr);
	~KMenuManager() override;

	Q_INVOKABLE void showMenu(QQuickItem *, QQuickItem *, int);

private:
	KMenu *m_menu; // Pointer to our actual KMenu object
	
	QPoint adjustedMenuPosition(QQuickItem *, QQuickItem *, int);
};
