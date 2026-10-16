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
	/* Class which creates various custom QActions to be used in QMenus */
	Q_OBJECT
	QML_ELEMENT
public:
	explicit ActionFactory(QObject *parent = nullptr);

	// Returns a pointer to a PopupTitle (port of KDE 3's KPopupTitle) QAction
	Q_INVOKABLE QAction *popupTitle(QObject *parent, const QString &title, const QString &icon = QString());
};
