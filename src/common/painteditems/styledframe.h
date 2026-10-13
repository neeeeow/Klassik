/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "painteditem.h"

class StyledFrame : public PaintedItem
{
	/*
	   QML element which draws a frame using QStyle
	*/
	  
	Q_OBJECT
	QML_ELEMENT
	// Line width to be used by the frame. Set -1 (default value) to have the line
	// width set by the active QStyle
	Q_PROPERTY(int lineWidth MEMBER m_lineWidth NOTIFY propertyChanged)
public:
	explicit StyledFrame(QQuickItem *parent = nullptr) : PaintedItem(parent) {}
	
private:
	int m_lineWidth = -1;
	
    void paint(QPainter *p) const override {
		if (!style())
			return;

		QStyleOptionFrame frame;
		frame.QStyleOption::operator=(baseStyleOption());
		frame.frameShape = QFrame::StyledPanel;
		if (m_lineWidth >= 0)
			frame.lineWidth = m_lineWidth;
		else
			frame.lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
		if (!isSunken())
			frame.state |= QStyle::State_Raised;	

		style()->drawControl(QStyle::CE_ShapedFrame, &frame, p);
	}
};
