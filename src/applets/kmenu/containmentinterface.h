/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>
#include <QPointer>

#include <KService>

namespace Plasma
{
    class Containment;
    class Applet;
}

class ContainmentInterface : public QObject
{
	Q_OBJECT
	
public:
	enum Target {
		Desktop = 0,
		Panel,
		Quicklaunch
	};
	
	explicit ContainmentInterface(Plasma::Applet *applet);

	bool mayAddLauncher(ContainmentInterface::Target target) const;
	void addLauncher(ContainmentInterface::Target target, const KService::Ptr &service) const;

private:
	Plasma::Applet *m_applet = nullptr;

	Plasma::Containment *containment() const;
	Plasma::Applet *findQuicklaunchApplet() const;
};
