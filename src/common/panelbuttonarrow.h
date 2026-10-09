/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "painteditem.h"

class PanelButtonArrow : public PaintedItem
{
	/*
	   This class draws a sunken KDE 3 style popup menu arrow for buttons
	*/
	  
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(bool active READ active WRITE setActive)
	Q_PROPERTY(MenuLocation location READ location WRITE setLocation)
public:
	explicit PanelButtonArrow(QQuickItem *parent = nullptr);

	enum MenuLocation {
		Above,
		Below,
		Right,
		Left
	};
	Q_ENUM(MenuLocation)

	bool active() const { return m_active; }
	void setActive(bool state) {
		if (m_active != state) {
			m_active = state;
		    requestRepaint();
		}
	}

	MenuLocation location() const { return m_location; }
	void setLocation(MenuLocation location) {
		if (m_location != location) {
			m_location = location;
		    requestRepaint();
		}
	}

private:
	MenuLocation m_location = Above;
	bool m_active = false;

	void paint(QPainter *p) const override;
};
