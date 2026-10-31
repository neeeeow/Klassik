/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "servicegroupmenuapplet.h"
#include "servicegroupmenu.h"

ServiceGroupMenuApplet::ServiceGroupMenuApplet(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args)
	: ServiceMenuApplet(parentObject, data, args) {}

ServiceMenu *
ServiceGroupMenuApplet::createMenu()
{
	return new ServiceGroupMenu(this);
}

K_PLUGIN_CLASS_WITH_JSON(ServiceGroupMenuApplet, "metadata.json")

#include "servicegroupmenuapplet.moc"
