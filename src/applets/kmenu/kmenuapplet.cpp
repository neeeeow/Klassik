/*
  SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "kmenuapplet.h"
#include "kmenu.h"

#include <KPluginFactory>

KMenuApplet::KMenuApplet(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args)
	: ServiceMenuApplet(parentObject, data, args)
{
	setInfo(KServiceGroup::root());
}

ServiceMenu *
KMenuApplet::createMenu()
{
	return new KMenu(this);
}

K_PLUGIN_CLASS_WITH_JSON(KMenuApplet, "metadata.json")

#include "kmenuapplet.moc"
