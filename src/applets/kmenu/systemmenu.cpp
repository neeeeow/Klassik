/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "systemmenu.h"
#include "kmenuapplet.h"

#include <KFilePlacesModel>

SystemMenu::SystemMenu(KMenuApplet *applet, QWidget *parent)
	: ServiceMenu(applet, parent)
{
	initialize();
}

SystemMenu::SystemMenu(const QString &title, KMenuApplet *applet, QWidget *parent)
	: ServiceMenu(title, applet, parent)
{
	initialize();
}

void
SystemMenu::initialize()
{
	if (initialized()) return;
	ServiceMenu::initialize();

	const KFilePlacesModel placesModel;
	for (int i = 0; i < placesModel.rowCount(); ++i) {
		const QModelIndex idx = placesModel.index(i, 0);
        if (placesModel.isHidden(idx))
            continue;

		QString title = idx.data(Qt::DisplayRole).toString();
        const QIcon &icon = idx.data(Qt::DecorationRole).value<QIcon>();
        const QUrl &url = idx.data(KFilePlacesModel::UrlRole).toUrl();

		if (QAction *action = createActionFromUrl(url)) {
			action->setText(title.replace(QLatin1Char('&'), QStringLiteral("&&")));
			action->setIcon(icon);
			addAction(action);
		}
	}
	
	setInitialized(true);
}
