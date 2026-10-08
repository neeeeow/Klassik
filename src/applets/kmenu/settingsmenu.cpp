/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "settingsmenu.h"

#include <KPluginMetaData>
#include <KAuthorized>
#include <KFileUtils>
#include <KDesktopFile>
#include <KLocalizedString>
#include <KConfigGroup>

SettingsMenu::SettingsMenu(KMenuApplet *applet, QWidget *parent)
	: ServiceMenu(applet, parent)
{
	initialize();
}

SettingsMenu::SettingsMenu(const QString &title, KMenuApplet *applet, QWidget *parent)
	: ServiceMenu(title, applet, parent)
{
	initialize();
}

void
SettingsMenu::initialize()
{
	if (initialized()) return;
	ServiceMenu::initialize();

	// Add system settings action at the top of the menu
	const KService::Ptr systemsettings = KService::serviceByDesktopName(QStringLiteral("systemsettings"));
	if (QAction *settingsAction = createActionFromService(systemsettings)) {
		addAction(settingsAction);
		addSeparator();
	}

	// Load KCM meta data
	const QList<KPluginMetaData> pluginModules = findKCMsMetaData();

	// Load category data
	const QList<CategoryData> categories = parseCategoryFiles();
	
	// Populate our settings tree
	SettingsItem root;
	root.isCategory = true;
	buildSettingsTree(&root, pluginModules, categories);

	// Populate the menu itself
	if (!root.children.isEmpty())		
		populateMenu(&root, this);

	// Add the menu editor action at the bottom
	QAction *editAction = addAction(QIcon::fromTheme(QStringLiteral("kmenuedit")), i18n("Menu Editor"));
	connect(editAction, &QAction::triggered, this, []() {
		runMenuEditor();
	});
	
	setInitialized(true);
}

QList<SettingsMenu::CategoryData>
SettingsMenu::parseCategoryFiles()
{
	const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("systemsettings/categories"), QStandardPaths::LocateDirectory);
    const QStringList categoryFiles = KFileUtils::findAllUniqueFiles(dirs, QStringList(QStringLiteral("*.desktop")));

	QList<CategoryData> categories;
	for (const QString &categoryFile : categoryFiles) {
		const KDesktopFile file(categoryFile);
		const KConfigGroup entry = file.desktopGroup();

		CategoryData category;
		category.id = entry.readEntry("X-KDE-System-Settings-Category");
		if (category.id.isEmpty() || category.id == QStringLiteral("rootcategory"))
			continue;

		category.name = entry.readEntry("Name");
		category.icon = entry.readEntry("Icon");
		category.parentCategory = entry.readEntry("X-KDE-System-Settings-Parent-Category");
		category.parentCategory2 = entry.readEntry("X-KDE-System-Settings-Parent-Category-V2");
		category.weight = entry.readEntry("X-KDE-Weight", 100);

		categories.append(category);
	}

	return categories;
}

QList<KPluginMetaData>
SettingsMenu::findKCMsMetaData()
{
	QList<KPluginMetaData> modules;
	std::set<QString> uniquePluginIds;

	QList<KPluginMetaData> metaDataList = KPluginMetaData::findPlugins(QStringLiteral("plasma/kcms"));
	metaDataList << KPluginMetaData::findPlugins(QStringLiteral("plasma/kcms/systemsettings"));
	metaDataList << KPluginMetaData::findPlugins(QStringLiteral("plasma/kcms/systemsettings_qwidgets"));
	for (const auto &m : std::as_const(metaDataList)) {
		if (!KAuthorized::authorizeControlModule(m.pluginId()))
            continue;
		modules << m;
		const bool inserted = uniquePluginIds.insert(m.pluginId()).second;
		if (!inserted)
            qWarning() << "the plugin" << m.pluginId() << " was found in multiple namespaces";
	}
	std::stable_sort(modules.begin(), modules.end(), [](const KPluginMetaData &m1, const KPluginMetaData &m2) {
        return QString::compare(m1.pluginId(), m2.pluginId(), Qt::CaseInsensitive) < 0;
    });

	return modules;
}

void
SettingsMenu::buildSettingsTree(SettingsItem *parent,
								const QList<KPluginMetaData> &pluginModules,
							    const QList<CategoryData> &categories)
{
	// Look for categories
	for (const CategoryData &category : std::as_const(categories)) {
		if (category.id == parent->id)
            continue;

		if (category.parentCategory == parent->id ||
			// V2 entries must not be empty if they want to become a proper category.
            (!category.parentCategory2.isEmpty() && category.parentCategory2 == parent->id)) {
			
			// Create category settings item
			auto item = new SettingsItem();
			item->isCategory = true;
			item->id = category.id;
			item->name = category.name;
			item->icon = category.icon;
			item->weight = category.weight;
			parent->children.append(item);
			buildSettingsTree(item, pluginModules, categories); // Recurse for the new item
		}
	}

	// Add KCMs
	for (const auto &metaData : std::as_const(pluginModules)) {
		const QString parentCategory = metaData.value(QStringLiteral("X-KDE-System-Settings-Parent-Category"));
		const QString parentCategory2 = metaData.value(QStringLiteral("X-KDE-System-Settings-Parent-Category-V2"));

		if (!parent->id.isEmpty() &&
			(parentCategory == parent->id || parentCategory2 == parent->id)) {

			// Create KCM (i.e. isCategory = false) settings item
			auto item = new SettingsItem();
			item->isCategory = false;
			item->id = metaData.pluginId();
			item->name = metaData.name();
			item->icon = metaData.iconName();
			item->weight = metaData.value(QStringLiteral("X-KDE-Weight"), 100);
			parent->children.append(item);
		}
	}

	// Sort children by weight
	std::stable_sort(parent->children.begin(), parent->children.end(), [](const SettingsItem *i1, const SettingsItem *i2) {
		if (!i1 || !i2)
			return i1 < i2;
		return i1->weight < i2->weight;
	});
}

void
SettingsMenu::populateMenu(const SettingsItem *item, ServiceMenu *menu)
{
	for (SettingsItem *child : std::as_const(item->children)) {
		if (child->isCategory) {
			// If it's a category, add a sub menu for each child and recurse
			if (!child->children.isEmpty()) {
				QString title = child->name;
				auto *subMenu = new ServiceMenu(title.replace(QLatin1Char('&'), QStringLiteral("&&")), applet(), menu);
				subMenu->initialize();
				subMenu->setIcon(QIcon::fromTheme(child->icon));
				menu->addMenu(subMenu);
				populateMenu(child, subMenu);
			}
		} else {
			// If it's not a category, then it's a KCM, so add it to the menu
			if (QAction *action = menu->createActionFromKCM(child->id, child->name, child->icon))
				menu->addAction(action);
		}
	}
}
