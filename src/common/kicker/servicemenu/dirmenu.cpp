/*
  SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "dirmenu.h"
#include "popuptitle.h"

#include <QDir>
#include <QFileInfo>

DirMenu::DirMenu(const ItemInfo &info, Plasma::Applet *applet, QWidget *parent)
	: ServiceMenu(info, applet, parent), m_info(info) {}

void
DirMenu::populate()
{
	m_info.refresh();
	if (m_info.type() != ItemInfo::Type::Directory) {
		addAction(noEntriesAction());
		return;
	}

	const QDir dir(m_info.localPath());
	const auto entries = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot,
										   QDir::DirsFirst | QDir::Name | QDir::IgnoreCase | QDir::LocaleAware);
	
	addAction(new PopupTitle(icon(), dir.absolutePath(), this));
	if (QAction *fileExplorerAction = openInFileExplorerAction(m_info, this)) {
		addAction(fileExplorerAction);
		if (!entries.isEmpty())
			addSeparator();
	}

	bool seenDir = false, fileSeparator = false;
	for (const QFileInfo &file : entries) {
		const ItemInfo info = ItemInfo(QUrl::fromLocalFile(file.absoluteFilePath()));
		if (QAction *action = createActionFromItemInfo(info)) {
			if ((info.type() == ItemInfo::Type::Directory) || (info.type() == ItemInfo::Type::ServiceGroup))
				seenDir = true;
			else if (seenDir && !fileSeparator) {
				addSeparator();
				fileSeparator = true;
			}
			addAction(action);
		}
	}
}
