/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "systemmenu.h"

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

	if (m_placesModel) {
		m_placesModel->disconnect();
		m_placesModel->deleteLater();
	}
	m_placesModel = new KFilePlacesModel(this);

	// We must connect to aboutToShow and populate the menu just before displaying it.
	// We avoid connectng the KFilePlacesModel, since its signals fire many times per
	// second, and thus markDirty() would be called very often - which is unnecessary
	connect(this, &QMenu::aboutToShow, this, &SystemMenu::populate);
	
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

		QString title = idx.data(Qt::DisplayRole).toString();
        const QIcon &icon = idx.data(Qt::DecorationRole).value<QIcon>();
        const QUrl &url = idx.data(KFilePlacesModel::UrlRole).toUrl();

		if (QAction *action = createActionFromUrl(url)) {
			action->setText(title.replace(QLatin1Char('&'), QStringLiteral("&&")));
			action->setIcon(icon);
			addAction(action);
		}
	}
}
