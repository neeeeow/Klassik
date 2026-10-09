/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "panelbuttonarrow.h"

PanelButtonArrow::PanelButtonArrow(QQuickItem *parent) : PaintedItem(parent)
{
}

void
PanelButtonArrow::paint(QPainter *p) const
{
	if (!style())
		return;  
		
	// Here we use the slightly older KDE 3 panel button arrow logic, it
	// looks better than the new style adopted later in 3.5's life.
	QStyle::PrimitiveElement e = QStyle::PE_IndicatorArrowUp;
	const int arrowSize = style()->pixelMetric(QStyle::PM_MenuButtonIndicator);
	QRect r(0, 0, arrowSize, arrowSize);
	switch (m_location) {
	case Above:
		e = QStyle::PE_IndicatorArrowUp;
		break;
	case Below:
		e = QStyle::PE_IndicatorArrowDown;
		r.translate(0, rect().height() - arrowSize);
		break;
	case Right:
		e = QStyle::PE_IndicatorArrowRight;
		r.translate(rect().width() - arrowSize, 0);
		break;
	case Left:
		e = QStyle::PE_IndicatorArrowLeft;
		break;
	}

	QStyleOption arrow;
	arrow.rect = r;
	arrow.palette = QGuiApplication::palette();
	arrow.state = QStyle::State_Enabled;
	if (m_active)
		arrow.state |= QStyle::State_Sunken;
	style()->drawPrimitive(e, &arrow, p);
}
