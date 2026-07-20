/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "klassikqstyleitem.h"

#include <QPalette>
#include <QStyleOptionFrame>
#include <QPainter>

class PanelBackground : public KlassikQStyleItem
{
	/*
	  This class draws a sunken KDE 3 style panel background
	*/
	  
	Q_OBJECT
	QML_ELEMENT
public:
	PanelBackground(QQuickItem *parent = nullptr) : KlassikQStyleItem(parent) {}
	~PanelBackground() override = default;

protected:	
	void paint(QPainter *p) const override {		
		QRect r = QRect(0,0,width(),height());
		QPalette pal = QGuiApplication::palette();

		p->fillRect(r, pal.window());

		if (!m_style)
			return;
		
		QStyleOptionFrame opt;
		opt.palette = pal;
		opt.rect = r;
		opt.state = QStyle::State_Enabled | QStyle::State_Raised;
		opt.features = QStyleOptionFrame::None;
		opt.frameShape = QFrame::StyledPanel;
		opt.lineWidth = 2;

		m_style->drawControl(QStyle::CE_ShapedFrame, &opt, p);
	}
};
