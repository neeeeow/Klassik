/*
  SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicemenu.h"

class KPluginMetaData;

class SettingsMenu final : public ServiceMenu
{
	Q_OBJECT
	
public:
	explicit SettingsMenu(Plasma::Applet *applet, QWidget *parent = nullptr);	

protected:
	void initialize() override;
	
private:
	// Struct containing the data from a .desktop file
	struct CategoryData {
		QString id;
		QString name;
		QString icon;
		QString parentCategory;
		QString parentCategory2;
		int weight = 100;
	};
	
	// Basic struct containing the data of a settings menu item, including its children. Starting
	// from a blank item, it is possible to construct a tree containing every setting in the correct
	// hierarchy.
	struct SettingsItem {
		Q_DISABLE_COPY(SettingsItem)
		SettingsItem() = default;
		
		bool isCategory = false;
		QString id;
		QString name;
		QString icon;
		int weight = 100; // default weight
    
		QList<std::shared_ptr<SettingsItem>> children;
	};

	// Private constructor to skip building the SettingsItem tree. Should never be called
	// outside of this class
	SettingsMenu(std::shared_ptr<const SettingsItem> item, Plasma::Applet *applet, QWidget *parent = nullptr);

	// Whether or not this menu is the root-level settings menu. NB: the root item
	// always has an empty id.
    bool isRoot() const { return !m_item || m_item->id.isEmpty(); }

	void populate() override;
	
	// Parses each .desktop file to retrieve the category data.
	static QList<CategoryData> parseCategoryFiles();
	
	// Fetches the metadata for each KCM
	static QList<KPluginMetaData> findKCMsMetaData();

	// Builds the settings item hierarchy from a starting item
	static void buildSettingsTree(SettingsItem *parent, const QList<KPluginMetaData> &pluginModules,
								  const QList<CategoryData> &categories);

	// Adds a settingsitem (and all of its children) to our menu.
	void addSettingsActions();

    // QAction* which launches a KCM with given id inside of
	// kcmshell6, with a given name and icon, parented to this.
	QAction* createActionFromKCM(const QString &id, const QString &name,
								 const QString &icon);
	
	// The SettingsItem used for populating the menu
	std::shared_ptr<const SettingsItem> m_item;
};
