/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "systemmenu.h"

#include <KFilePlacesModel>

SystemMenu::SystemMenu(ServiceMenuApplet *applet, QWidget *parent)
	: SystemMenu(QIcon(), QString(), applet, parent) {}

SystemMenu::SystemMenu(const QString &title, ServiceMenuApplet *applet, QWidget *parent)
	: SystemMenu(QIcon(), title, applet, parent) {}

SystemMenu::SystemMenu(const QIcon &icon, const QString &title, ServiceMenuApplet *applet, QWidget *parent)
	: ServiceMenu(icon, title, applet, parent)
{
	setContextMenuPolicy(Qt::NoContextMenu); // We don't want the context menu appearing here
}

void
SystemMenu::rebuild()
{
	if (m_placesModel)
		m_placesModel->deleteLater();
	m_placesModel = new KFilePlacesModel(this);
}

void
SystemMenu::populate()
{
	if (!m_placesModel)
		return;
	for (int i = 0; i < m_placesModel->rowCount(); ++i) {
		const QModelIndex idx = m_placesModel->index(i, 0);
		if (m_placesModel->isHidden(idx))
			continue;

		const QString title = idx.data(Qt::DisplayRole).toString();
		const QIcon icon = idx.data(Qt::DecorationRole).value<QIcon>();
		const QUrl url = idx.data(KFilePlacesModel::UrlRole).toUrl();

		if (QAction *action = createActionFromUrl(url)) {
			action->setText(escapeMnemonics(title));
			action->setIcon(icon);
			addAction(action);
		}
	}
}
