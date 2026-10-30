/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "dirmenu.h"
#include "popuptitle.h"

#include <QDirIterator>

#include <KFileItem>
#include <KProtocolManager>
#include <KLocalizedString>

DirMenu::DirMenu(const QUrl &url, ServiceMenuApplet *applet, QWidget *parent)
	: ServiceMenu(applet, parent), m_url(url)
{
	if (!m_url.isValid())
		return;
	KFileItem fileItem(m_url);
	setTitle(escapeMnemonics(fileItem.text()));
    setIcon(QIcon::fromTheme(fileItem.iconName(), QIcon::fromTheme(QStringLiteral("folder"))));
}

void
DirMenu::populate()
{
	if (!m_url.isValid() ||
		!m_url.isLocalFile() ||
		!KFileItem(m_url).isDir() ||
		!KProtocolManager::supportsListing(m_url))
		return;

	QDir dir(m_url.toLocalFile());
	dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
	
	addAction(new PopupTitle(icon(), dir.absolutePath(), this));
	if (QAction *fileExplorerAction = createFileExplorerActionFromUrl(m_url)) {
		addAction(fileExplorerAction);
		if (!dir.isEmpty())
			addSeparator();
	}

	QDirIterator it(dir);
	while (it.hasNext()) {
		const QUrl url = QUrl::fromLocalFile(it.next());
		if (it.fileInfo().isDir() && url.isLocalFile() && KProtocolManager::supportsListing(url)) {
			// If the URL points to a directory, we add a sub menu for the new directory
			QAction *action = addMenu(new DirMenu(url, applet(), this));
			action->setData(url);
			action->setToolTip(url.toDisplayString(QUrl::PreferLocalFile));
		} else {
			// Otherwise, just add a normal action
			if (QAction *action = createActionFromUrl(url))
				addAction(action);
		}
	}
}
