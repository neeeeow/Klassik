#pragma once

#include "servicemenu.h"

#include <QObject>

#include <PlasmaActivities/Stats/ResultModel>

class SettingsMenu : public ServiceMenu
{
	Q_OBJECT;
	
public:
	explicit SettingsMenu(KMenuApplet *applet, QWidget *parent = nullptr);
	explicit SettingsMenu(const QString &title, KMenuApplet *applet, QWidget *parent = nullptr);
	~SettingsMenu() override = default;

private:
	void initialize() override;
	void updateSettingsMenu();

	KActivities::Stats::ResultModel *m_settingsList = nullptr;
};
