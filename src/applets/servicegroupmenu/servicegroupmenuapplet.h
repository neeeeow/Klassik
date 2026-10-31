/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicemenuapplet.h"

class ServiceGroupMenuApplet : public ServiceMenuApplet
{
	Q_OBJECT
	
public:
	ServiceGroupMenuApplet(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args);

private:
	ServiceMenu* createMenu() override;
};
