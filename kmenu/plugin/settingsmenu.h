#pragma once

#include "servicemenu.h"

#include <QObject>

#include <PlasmaActivities/Stats/ResultModel>

class SettingsMenu : public ServiceMenu
{
	Q_OBJECT;
	
public:
	explicit SettingsMenu(Plasma::Containment *containment, QWidget *parent = nullptr);
	explicit SettingsMenu(const QString &title, Plasma::Containment *containment, QWidget *parent = nullptr);
	~SettingsMenu() override;

private:
	void initialize();
	void updateSettingsMenu();

	KActivities::Stats::ResultModel *m_settingsList = nullptr;
};
