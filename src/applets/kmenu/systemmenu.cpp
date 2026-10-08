/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "systemmenu.h"

#include <KFilePlacesModel>

SystemMenu::SystemMenu(KMenuApplet *applet, QWidget *parent)
	: SystemMenu(QString(), applet, parent)
{
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

	if (m_placesModel) {
		m_placesModel->disconnect(this);
		m_placesModel->deleteLater();
	}
	m_placesModel = new KFilePlacesModel(this);
	connectModel(m_placesModel);
	populate();
	setInitialized(true);
}

void
SystemMenu::populate()
{
	clear();
	if (!m_placesModel)
		return;
	for (int i = 0; i < m_placesModel->rowCount(); ++i) {
		const QModelIndex idx = m_placesModel->index(i, 0);
        if (m_placesModel->isHidden(idx))
            continue;

		const QString &title = idx.data(Qt::DisplayRole).toString();
        const QIcon &icon = idx.data(Qt::DecorationRole).value<QIcon>();
        const QUrl &url = idx.data(KFilePlacesModel::UrlRole).toUrl();

		if (QAction *action = createActionFromUrl(url)) {
			action->setText(escapeMnemonics(title));
			action->setIcon(icon);
			addAction(action);
		}
	}
}
