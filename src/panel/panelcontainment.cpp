/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "panelcontainment.h"

#include <KPluginFactory>

K_PLUGIN_CLASS_WITH_JSON(PanelContainment, "metadata.json")

PanelContainment::PanelContainment(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args)
	: Plasma::Containment(parentObject, data, args)
{
}

#include "panelcontainment.moc"
