/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "settingsmenu.h"
#include "kmenuapplet.h"

#include <KPluginFactory>
#include <KAuthorized>
#include <KFileUtils>
#include <KDesktopFile>
#include <KLocalizedString>

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
	if (QAction *settingsAction = createActionFromService(systemsettings))
		addAction(settingsAction);

	// Load KCM meta data
	m_pluginModules = findKCMsMetaData();

	// Load category data
	const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("systemsettings/categories"), QStandardPaths::LocateDirectory);
	m_categories = KFileUtils::findAllUniqueFiles(dirs, QStringList(QStringLiteral("*.desktop")));

	// Populate our settings tree
	SettingsItem root;
	root.isCategory = true;
	buildSettingsTree(&root);

	// Populate the menu itself
	if (!root.children.isEmpty()) {
		addSeparator();
		populateMenu(&root, this);
	}

	// Add the menu editor action at the bottom
	QAction *editAction = addAction(QIcon::fromTheme(QStringLiteral("kmenuedit")), i18n("Menu Editor"));
	connect(editAction, &QAction::triggered, this, []() {
		runMenuEditor();
	});
	
	setInitialized(true);
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
SettingsMenu::buildSettingsTree(SettingsItem *parent)
{
	// Look for categories
	for (const QString &category : std::as_const(m_categories)) {
		const KDesktopFile file(category);
		const KConfigGroup entry = file.desktopGroup();
		const QString parentCategory = entry.readEntry("X-KDE-System-Settings-Parent-Category");
		const QString parentCategory2 = entry.readEntry("X-KDE-System-Settings-Parent-Category-V2");

		if (parentCategory == parent->id ||
			// V2 entries must not be empty if they want to become a proper category.
			(!parentCategory2.isEmpty() && parentCategory2 == parent->id)) {

			const QString id = entry.readEntry("X-KDE-System-Settings-Category");
			if (id == QStringLiteral("rootcategory"))
				continue; // skip the root category

			// Create category settings item
			auto item = new SettingsItem();
			item->isCategory = true;
			item->id = id;
			item->name = entry.readEntry("Name");
			item->icon = entry.readEntry("Icon");
			item->weight = entry.readEntry("X-KDE-Weight", 100);
			parent->children.append(item);
			buildSettingsTree(item); // Recurse for the new item
		}
	}

	// Add KCMs
	for (const auto &metaData : std::as_const(m_pluginModules)) {
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
				auto *subMenu = new ServiceMenu(child->name.replace(QLatin1Char('&'), QStringLiteral("&&")), applet(), menu);
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
