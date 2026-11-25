/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "painteditem.h"

class StyledFocusRect : public PaintedItem
{
	/*
	  QML element which draws a PE_FrameFocusRect
	*/
	Q_OBJECT
	QML_ELEMENT
public:
	explicit StyledFocusRect(QQuickItem *parent = nullptr) : PaintedItem(parent) {}
private:
	void paint(QPainter *p) const override {
		if (!style())
			return;

		QStyleOptionFocusRect fr;
		fr.QStyleOption::operator=(baseStyleOption());
		fr.backgroundColor = fr.palette.color(QPalette::Window);
		style()->drawPrimitive(QStyle::PE_FrameFocusRect, &fr, p);
	}
};
