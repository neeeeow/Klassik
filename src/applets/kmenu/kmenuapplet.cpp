/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "kmenuapplet.h"
#include "kmenu.h"

K_PLUGIN_CLASS_WITH_JSON(KMenuApplet, "metadata.json")

KMenuApplet::KMenuApplet(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args)
	: ServiceMenuApplet(parentObject, data, args) {}

ServiceMenu *
KMenuApplet::createMenu()
{
	return new KMenu(this);
}

#include "kmenuapplet.moc"
