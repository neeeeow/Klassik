/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "painteditem.h"

#include <QStyleOptionToolButton>
#include <QPainter>

class StyledToolButton : public PaintedItem
{
	/*
	  This class draws a QStyle tool button.
	  NB: QQC2 does deligate the drawing of tool buttons to QStyle by default,
	  but it does not use the QML Item's palette for the tool button, whereas
	  here we do use the Item's palette.
	*/
	  
	Q_OBJECT
	QML_ELEMENT
public:
    explicit StyledToolButton(QQuickItem *parent = nullptr) : PaintedItem(parent) {}

private:
	void paint(QPainter *p) const override {		
		if (!style())
			return;

		// Use a regular QStyleOption for the button, no need to use QStyleOptionToolButton here
		QStyleOption button = baseStyleOption();
		style()->drawPrimitive(QStyle::PE_PanelButtonTool, &button, p);
	}
};
