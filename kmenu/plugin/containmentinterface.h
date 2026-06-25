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
	
	explicit ContainmentInterface(Plasma::Containment *containment, QObject *parent = nullptr);
	~ContainmentInterface() override;

	Plasma::Containment* containmentPtr() const { return m_containment; }

	bool mayAddLauncher(ContainmentInterface::Target target);
	bool hasLauncher(ContainmentInterface::Target target, const KService::Ptr &service);
	void addLauncher(ContainmentInterface::Target target, const KService::Ptr &service);

private:
	QStringList m_knownTaskManagers{
		QLatin1String("org.kde.plasma.taskmanager"),
			QLatin1String("org.kde.plasma.icontasks"),
			QLatin1String("org.kde.plasma.expandingiconstaskmanager"),
			};
	
	Plasma::Containment *m_containment;

	Plasma::Applet *findTaskManagerApplet();
};
