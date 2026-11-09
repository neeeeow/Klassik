/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QMenu>
#include <QPointer>

namespace Plasma {
    class Applet;
}

class PanelMenu : public QMenu
{
	/*
	  Base class for menu which lives on a Plasma applet/containment. Actions are repopulated every
	  time the menu is shown. Useful for things like application menus.
	*/
	Q_OBJECT
public:
	explicit PanelMenu(Plasma::Applet *applet, QWidget *parent = nullptr);
	PanelMenu(const QString &title, Plasma::Applet *applet, QWidget *parent = nullptr);
	PanelMenu(const QIcon &icon, const QString &title, Plasma::Applet *applet, QWidget *parent = nullptr);

private:
	// Pointer to the plasma applet
	QPointer<Plasma::Applet> m_applet;
};
