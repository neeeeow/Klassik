/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "panelmenu.h"

#include <Plasma/Applet>

PanelMenu::PanelMenu(Plasma::Applet *applet, QWidget *parent)
: PanelMenu(QString(), applet, parent) {}

PanelMenu::PanelMenu(const QString &title, Plasma::Applet *applet, QWidget *parent)
: PanelMenu(QIcon(), title, applet, parent) {}

PanelMenu::PanelMenu(const QIcon &icon, const QString &title, Plasma::Applet *applet, QWidget *parent)
	: QMenu(title, parent),
	  m_applet(applet)
{
	if (!icon.isNull())
		setIcon(icon);	
	setContextMenuPolicy(Qt::CustomContextMenu);
	/*connect(this, &QWidget::customContextMenuRequested, this, &ServiceMenu::showContextMenu);
	connect(this, &QMenu::aboutToShow, this, &ServiceMenu::refreshContents);
	connect(this, &QMenu::aboutToHide, this, [this] {
		m_fresh = false;
		m_startPos.reset();
		});*/
}
