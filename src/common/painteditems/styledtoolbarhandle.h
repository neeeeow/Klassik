/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "painteditem.h"

class StyledToolBarHandle : public PaintedItem
{
	/*
	  This class draws a QStyle toolbar handle
	*/
	  
	Q_OBJECT
	QML_ELEMENT
	// Whether or not to apply the QStyle::State_Horizontal flag to the handle 
	Q_PROPERTY(bool horizontal MEMBER m_horizontal NOTIFY propertyChanged)
	// The width of the handle
	Q_PROPERTY(int handleExtent READ handleExtent NOTIFY propertyChanged)
public:
    StyledToolBarHandle(QQuickItem *parent = nullptr) : PaintedItem(parent) {}
	int handleExtent() const {
		if (!style()) return 0;
		return style()->pixelMetric(QStyle::PM_ToolBarHandleExtent);
	}

private:
	bool m_horizontal = true;

	void paint(QPainter *p) const override {		
		if (!style())
			return;

		QStyleOption handle = baseStyleOption();
		if (m_horizontal)
			handle.state |= QStyle::State_Horizontal;
		style()->drawPrimitive(QStyle::PE_IndicatorToolBarHandle, &handle, p);
	}
};
