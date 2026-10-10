/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "painteditem.h"

class GrabHandle : public PaintedItem
{
	/*
	  This class draws a grab handle for panel applets
	*/
	  
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(bool horizontal MEMBER m_horizontal NOTIFY propertyChanged)
public:
    GrabHandle(QQuickItem *parent = nullptr) : PaintedItem(parent) {}
    ~GrabHandle() override = default;

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
