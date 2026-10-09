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
	frame.palette = QGuiApplication::palette();
	frame.rect = rect();
	frame.state = QStyle::State_Enabled | (m_sunken ? QStyle::State_Sunken : QStyle::State_Raised);
	frame.frameShape = QFrame::StyledPanel;
	frame.lineWidth = m_lineWidth;

	style()->drawControl(QStyle::CE_ShapedFrame, &frame, p);
}
