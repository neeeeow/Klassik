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
		TaskManager,
	};

	Q_ENUM(Target)
	
	explicit ContainmentInterface(Plasma::Applet *applet);
	~ContainmentInterface() override = default;

	bool mayAddLauncher(ContainmentInterface::Target target);
	bool hasLauncher(ContainmentInterface::Target target, const KService::Ptr &service);
	void addLauncher(ContainmentInterface::Target target, const KService::Ptr &service);

private:
	QStringList m_knownTaskManagers{
		QLatin1String("org.kde.plasma.taskmanager"),
			QLatin1String("org.kde.plasma.icontasks"),
			QLatin1String("org.kde.plasma.expandingiconstaskmanager"),
			};
	
	Plasma::Applet *m_applet = nullptr;

	Plasma::Applet *findTaskManagerApplet();
};
