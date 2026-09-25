/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

#include <KService>

#include <Plasma/Containment>

class ContainmentInterface : public QObject
{
	Q_OBJECT
	
public:
	enum Target {
		Desktop = 0,
		Panel,
		Quicklaunch
	};

	Q_ENUM(Target)
	
	explicit ContainmentInterface(Plasma::Applet *applet);
	~ContainmentInterface() override = default;

	bool mayAddLauncher(ContainmentInterface::Target target) const;
	void addLauncher(ContainmentInterface::Target target, const KService::Ptr &service) const;

private:
	Plasma::Containment *m_containment = nullptr;

	Plasma::Applet *findQuicklaunchApplet() const;
};
