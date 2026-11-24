/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QAction>
#include <QObject>
#include <QtQml/qqmlregistration.h>

class ActionFactory : public QObject
{
	/*
	  Class which creates various custom QActions to be used in PlasmaExtras.Menus inside of QML
	  NB: only place 'generic' actions in here, which are meant to be re-used between different
	  menus. Applet specific QActions should be generated in their Plasma::Applet C++ subclass	 
	 */
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON
public:
	explicit ActionFactory(QObject *parent = nullptr);

	// Returns a pointer to a PopupTitle (port of KDE 3's KPopupTitle) QAction
	Q_INVOKABLE QAction *popupTitle(const QString &icon, const QString &title, QObject *parent = nullptr);
	Q_INVOKABLE QAction *popupTitle(const QString &title, QObject *parent = nullptr);
};
