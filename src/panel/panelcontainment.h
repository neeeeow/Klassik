/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <Plasma/Containment>

class PanelContainment : public Plasma::Containment
{
	Q_OBJECT
public:
	PanelContainment(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args);
};
