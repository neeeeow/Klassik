/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "painteditem.h"

#include <Plasma/Plasma>

class HandleArrow : public PaintedItem
{
	/*
	  This class draws the arrow used in the panel handle
	*/
	  
	Q_OBJECT
	QML_ELEMENT
    Q_PROPERTY(Plasma::Types::Location panelLocation READ panelLocation WRITE setPanelLocation)
public:
    HandleArrow(QQuickItem *parent = nullptr) : PaintedItem(parent) {}
    ~HandleArrow() override = default;

    Plasma::Types::Location panelLocation() const { return m_panelLocation; }
	void setPanelLocation(Plasma::Types::Location location) {
		if (m_panelLocation != location) {
			m_panelLocation = location;
		    requestRepaint();
		}
	}

private:
    Plasma::Types::Location m_panelLocation = Plasma::Types::BottomEdge;

	void paint(QPainter *p) const override {		
		if (!style())
			return;

		QStyleOption arrow = baseStyleOption();

		QStyle::PrimitiveElement pe;
		switch (m_panelLocation) {
		case Plasma::Types::LeftEdge:
			pe = QStyle::PE_IndicatorArrowRight;
			break;	 
		case Plasma::Types::TopEdge:
			pe = QStyle::PE_IndicatorArrowDown;
			break;
		case Plasma::Types::RightEdge:
			pe = QStyle::PE_IndicatorArrowLeft;
			break;
		default:
			pe = QStyle::PE_IndicatorArrowUp;
			break;
		}
		
		style()->drawPrimitive(pe, &arrow, p);
	}
};
