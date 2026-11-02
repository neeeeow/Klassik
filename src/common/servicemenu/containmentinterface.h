/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>
#include <QPointer>

#include <KService>
#include <KServiceGroup>

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
	};
	
	explicit ContainmentInterface(Plasma::Applet *applet);

	bool mayAddItem(ContainmentInterface::Target target) const;
	void addLauncher(ContainmentInterface::Target target, const KService::Ptr &service) const;
	void addServiceGroupMenu(const KServiceGroup::Ptr &group) const;

private:	
	Plasma::Containment *containment() const;

	QPointer<Plasma::Applet> m_applet;
};
