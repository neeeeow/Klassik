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
	// Location of the plasma panel
	Q_PROPERTY(Plasma::Types::Location location MEMBER m_location NOTIFY propertyChanged REQUIRED)
public:
	explicit PanelButtonArrow(QQuickItem *parent = nullptr) : PaintedItem(parent) {}

private:
	Plasma::Types::Location m_location = Plasma::Types::Floating;

    void paint(QPainter *p) const override {
		if (!style())
			return;	 
		
		// Here we use the slightly older KDE 3 panel button arrow logic, it
		// looks better than the new style adopted later in 3.5's life.
		QStyle::PrimitiveElement e;
		const int arrowSize = style()->pixelMetric(QStyle::PM_MenuButtonIndicator);
		QRect r(0, 0, arrowSize, arrowSize);
		switch (m_location) {
		case Plasma::Types::TopEdge:
			e = QStyle::PE_IndicatorArrowDown;
			r.translate(0, rect().height() - arrowSize);
			break;
		case Plasma::Types::LeftEdge:
			e = QStyle::PE_IndicatorArrowRight;
			r.translate(rect().width() - arrowSize, 0);
			break;
		case Plasma::Types::RightEdge:
			e = QStyle::PE_IndicatorArrowLeft;
			break;
		case Plasma::Types::BottomEdge:
		default:
			e = QStyle::PE_IndicatorArrowUp;
			break;
		}

		QStyleOption arrow = baseStyleOption();
		arrow.rect = r;
		style()->drawPrimitive(e, &arrow, p);
	}
};
