/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "systemmenu.h"
#include "iteminfo.h"

#include <KLocalizedString>
#include <KFilePlacesModel>

SystemMenu::SystemMenu(Plasma::Applet *applet, QWidget *parent)
	: ServiceMenu(QIcon::fromTheme(QStringLiteral("computer")),
				  i18n("My System"), applet, parent) {}

void
SystemMenu::initialize()
{
	if (initialized()) return;
	ServiceMenu::initialize();
	
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

		if (!url.isValid())
			continue;
		
		auto *action = new QAction(icon, escapeMnemonics(title), this);
		action->setData(QVariant::fromValue(ItemInfo(url)));
		action->setToolTip(url.toDisplayString(QUrl::PreferLocalFile));	
		connect(action, &QAction::triggered, action, [url]() {
			execUrl(url);
		});
		addAction(action);
	}

	if (isEmpty())
		addAction(noEntriesAction());
}
