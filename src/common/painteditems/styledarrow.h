/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "painteditem.h"

#include <Plasma/Plasma>

class StyledArrow : public PaintedItem
{
	/*
	  This class draws an arrow using QStyle
	*/
	  
	Q_OBJECT
	QML_ELEMENT	
	// Direction in which the arrow should be facing. Directly corresponds to
	// QStyle arrow primitive elements
	Q_PROPERTY(StyledArrow::Direction direction MEMBER m_direction NOTIFY propertyChanged REQUIRED)
public:
	StyledArrow(QQuickItem *parent = nullptr) : PaintedItem(parent) {}

	enum Direction {
        Up = QStyle::PE_IndicatorArrowUp,
        Down = QStyle::PE_IndicatorArrowDown,
        Left = QStyle::PE_IndicatorArrowLeft,
        Right = QStyle::PE_IndicatorArrowRight
    };
    Q_ENUM(Direction)		

private:
	Direction m_direction = Up;

	void paint(QPainter *p) const override {		
		if (!style())
			return;
		QStyleOption arrow = baseStyleOption();
		style()->drawPrimitive(static_cast<QStyle::PrimitiveElement>(m_direction), &arrow, p);
	}
};
