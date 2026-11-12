/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "dirmenu.h"
#include "popuptitle.h"

#include <QDir>
#include <QFileInfo>

#include <KProtocolManager>

DirMenu::DirMenu(const KFileItem &dir, ServiceMenuApplet *applet, QWidget *parent)
	: ServiceMenu(applet, parent), m_dir(dir)
{
	if (m_dir.isNull() || !m_dir.isDir())
		return;
	setTitle(escapeMnemonics(m_dir.text()));
	setIcon(QIcon::fromTheme(m_dir.iconName(), QIcon::fromTheme(QStringLiteral("folder"))));
}

void
DirMenu::populate()
{
	m_dir.refresh();
	if (m_dir.isNull() ||
		!m_dir.isDir() ||
		!m_dir.isLocalFile() ||
		!KProtocolManager::supportsListing(m_dir.url())) {
		addAction(noEntriesAction());
		return;
	}

	const QDir dir(m_dir.localPath());
	const auto entries = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot,
										   QDir::DirsFirst | QDir::Name | QDir::IgnoreCase | QDir::LocaleAware);
	
	addAction(new PopupTitle(icon(), dir.absolutePath(), this));
	if (QAction *fileExplorerAction = openInFileExplorerAction(m_dir, this)) {
		addAction(fileExplorerAction);
		if (!entries.isEmpty())
			addSeparator();
	}

	bool seenDir = false, fileSeparator = false;
	for (const QFileInfo &file : entries) {
		if (file.isDir())
			seenDir = true;
		else if (seenDir && !fileSeparator) {
			addSeparator();
			fileSeparator = true;
		}
		
		const QUrl url = QUrl::fromLocalFile(file.absoluteFilePath());
		if (file.isDir() && url.isLocalFile() && KProtocolManager::supportsListing(url)) {
			QAction *action = addMenu(new DirMenu(KFileItem(url), applet(), this));
			action->setData(url);
			action->setToolTip(url.toDisplayString(QUrl::PreferLocalFile));
		} else if (QAction *action = createActionFromUrl(url))
			addAction(action);
	}
}
