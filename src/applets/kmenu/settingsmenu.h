/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicemenu.h"

class KPluginMetaData;

class SettingsMenu final : public ServiceMenu
{
	Q_OBJECT
	
public:
	explicit SettingsMenu(KMenuApplet *applet, QWidget *parent = nullptr);
	explicit SettingsMenu(const QString &title, KMenuApplet *applet, QWidget *parent = nullptr);
	~SettingsMenu() override = default;

protected:
	void initialize() override;
	
private:
	// Basic struct containing the data of a settings menu item, including its children. Starting
	// from a blank item, it is possible to construct a tree containing every setting in the correct
	// hierarchy.
	struct SettingsItem {
		bool isCategory = false;
		QString id;
		QString name;
		QString icon;
		int weight = 100; // default weight

		QList<SettingsItem*> children;

		~SettingsItem() {
			qDeleteAll(children);
		}
	};
	
	QStringList m_categories;
	QList<KPluginMetaData> m_pluginModules;

	// Fetches the metadata for each KCM
	QList<KPluginMetaData> findKCMsMetaData();

	// Builds the settings item hierarchy from a starting item
	void buildSettingsTree(SettingsItem *parent);

	// Adds a settingsitem (and all of its children) to a given menu
	void populateMenu(const SettingsItem *item, ServiceMenu *menu);
};
