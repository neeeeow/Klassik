/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "dirmenu.h"
#include "popuptitle.h"
#include "servicemenuapplet.h"

#include <QDir>
#include <QFileInfo>

#include <KFileItem>
#include <KProtocolManager>
#include <KLocalizedString>

DirMenu::DirMenu(const QUrl &url, ServiceMenuApplet *applet, QWidget *parent)
	: ServiceMenu(applet, parent), m_url(url)
{
	if (!m_url.isValid())
		return;
	KFileItem fileItem(m_url);
	setTitle(ServiceMenuApplet::escapeMnemonics(fileItem.text()));
	setIcon(QIcon::fromTheme(fileItem.iconName(), QIcon::fromTheme(QStringLiteral("folder"))));
}

void
DirMenu::populate()
{
	if (!m_url.isValid() ||
		!m_url.isLocalFile() ||
		!KFileItem(m_url).isDir() ||
		!KProtocolManager::supportsListing(m_url)) {
		QAction *emptyAction = addAction(i18n("No Entries"));
		emptyAction->setEnabled(false);
		return;
	}	

	const QDir dir(m_url.toLocalFile());
	const auto entries = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot,
										   QDir::DirsFirst | QDir::Name | QDir::IgnoreCase | QDir::LocaleAware);
	
	addAction(new PopupTitle(icon(), dir.absolutePath(), this));
	if (QAction *fileExplorerAction = ServiceMenuApplet::openInFileExplorerAction(m_url, this)) {
		addAction(fileExplorerAction);
		if (!entries.isEmpty())
			addSeparator();
	}

	bool seenDir, fileSeparator = false;
	for (const QFileInfo &file : entries) {
		if (file.isDir())
			seenDir = true;
		else if (seenDir && !fileSeparator) {
			addSeparator();
			fileSeparator = true;
		}
		
		const QUrl url = QUrl::fromLocalFile(file.absoluteFilePath());
		if (file.isDir() && url.isLocalFile() && KProtocolManager::supportsListing(url)) {
			QAction *action = addMenu(new DirMenu(url, applet(), this));
			action->setData(url);
			action->setToolTip(url.toDisplayString(QUrl::PreferLocalFile));
		} else if (QAction *action = createActionFromUrl(url))
			addAction(action);
	}
}
