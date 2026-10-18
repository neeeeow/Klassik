/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "actionfactory.h"
#include "popuptitle.h"

#include <QIcon>

ActionFactory::ActionFactory(QObject *parent) :
	QObject(parent)
{
}

QAction *
ActionFactory::popupTitle(QObject *parent, const QString &title, const QString &icon)
{
	if (icon.isEmpty())
		return new PopupTitle(title, parent);
	return new PopupTitle(QIcon::fromTheme(icon), title, parent);
}
