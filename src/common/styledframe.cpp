/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "styledframe.h"

#include <QStyleOptionFrame>

StyledFrame::StyledFrame(QQuickItem *parent)
	: PaintedItem(parent),
	  m_lineWidth(style() ? style()->pixelMetric(QStyle::PM_DefaultFrameWidth) : 1)
{
}

void
StyledFrame::paint(QPainter *p) const
{
	if (!style())
		return;

	QStyleOptionFrame frame;
    frame.QStyleOption::operator=(baseStyleOption());
	frame.frameShape = QFrame::StyledPanel;
	frame.lineWidth = m_lineWidth;
	if (!isSunken())
		frame.state |= QStyle::State_Raised;	

	style()->drawControl(QStyle::CE_ShapedFrame, &frame, p);
}
