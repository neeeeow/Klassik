/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "painteditem.h"

#include <Plasma/Plasma>

class PanelButtonArrow : public PaintedItem
{
	/*
	   This class draws a sunken KDE 3 style popup menu arrow for buttons
	*/
	  
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(Plasma::Types::Location location MEMBER m_location NOTIFY propertyChanged)
	Q_PROPERTY(bool active MEMBER m_active NOTIFY propertyChanged)
	Q_PROPERTY(bool mouseOver MEMBER m_mouseOver NOTIFY propertyChanged)	
public:
	explicit PanelButtonArrow(QQuickItem *parent = nullptr);

private:
	Plasma::Types::Location m_location = Plasma::Types::Floating;
	bool m_active = false;
	bool m_mouseOver = false;

	void paint(QPainter *p) const override;
};
