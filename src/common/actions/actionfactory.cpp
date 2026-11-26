/*
  SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "actionfactory.h"
#include "popuptitle.h"
#include "popupcalendar.h"

#include <QIcon>

ActionFactory::ActionFactory(QObject *parent) :
	QObject(parent) {}

QAction *
ActionFactory::popupTitle(const QString &icon, const QString &title, QObject *parent)
{
	return new PopupTitle(QIcon::fromTheme(icon), title, parent);
}

QAction *
ActionFactory::popupTitle(const QString &title, QObject *parent)
{
	return new PopupTitle(title, parent);
}

QAction *
ActionFactory::popupCalendar(QObject *parent)
{
	return new PopupCalendar(parent);
}
