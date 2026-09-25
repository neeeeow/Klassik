/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "sunkenappletframe.h"

#include <QStyleOptionFrame>

SunkenAppletFrame::SunkenAppletFrame(QQuickItem *parent) : PaintedItem(parent)
{
	// Retrieve default panel thickness
	if (style())
		m_lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
}

void
SunkenAppletFrame::paint(QPainter *p) const
{
	if (!style())
		return;

	QStyleOptionFrame frame;
	frame.palette = QGuiApplication::palette();
	frame.rect = QRect(0,0,width(),height());
	frame.state = QStyle::State_Enabled | QStyle::State_Sunken;
	frame.frameShape = QFrame::StyledPanel;
	frame.lineWidth = m_lineWidth;

	style()->drawControl(QStyle::CE_ShapedFrame, &frame, p);
}
