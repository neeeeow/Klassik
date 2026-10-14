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
	// Line width to be used by the frame. Leave unset to use the default value assigned
	// by the active QStyle
	Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth RESET resetLineWidth NOTIFY propertyChanged)
public:
	explicit StyledFrame(QQuickItem *parent = nullptr) : PaintedItem(parent) {}
	int lineWidth() const {
		if (m_lineWidth)
			return *m_lineWidth;
		return style() ? style()->pixelMetric(QStyle::PM_DefaultFrameWidth) : 0;
	}
	void setLineWidth(int lineWidth) {
		if (m_lineWidth != lineWidth) {
			m_lineWidth = lineWidth;
			Q_EMIT propertyChanged();
		}
	}
	void resetLineWidth() {
		if (!m_lineWidth)
			return;
		m_lineWidth.reset();
		Q_EMIT propertyChanged();
	}
	
private:
    std::optional<int> m_lineWidth;
	
    void paint(QPainter *p) const override {
		if (!style())
			return;

		QStyleOptionFrame frame;
		frame.QStyleOption::operator=(baseStyleOption());
		frame.frameShape = QFrame::StyledPanel;
		frame.lineWidth = lineWidth();
		if (!isSunken())
			frame.state |= QStyle::State_Raised;	

		style()->drawControl(QStyle::CE_ShapedFrame, &frame, p);
	}
};
