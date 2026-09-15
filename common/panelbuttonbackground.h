/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "klassikqstyleitem.h"

#include <QPalette>
#include <QStyleOptionFrame>

class PanelButtonBackground : public KlassikQStyleItem
{
	/*
	  This class draws a sunken KDE 3 style panel button background.
	  NOTE: the background here is always drawn as sunken, use the visible
	  QML property to set whether or not the background is sunken
	*/
	  
	Q_OBJECT
	QML_ELEMENT
public:
	PanelButtonBackground(QQuickItem *parent = nullptr) : KlassikQStyleItem(parent) {}
	~PanelButtonBackground() override = default;

protected:	
	void paint(QPainter *p) const override {		
		if (!m_style)
			return;

		QStyleOptionFrame opt;
		opt.palette = QGuiApplication::palette();
		opt.rect = QRect(0,0,width(),height());
		opt.state = QStyle::State_Enabled | QStyle::State_Sunken;
		opt.frameShape = QFrame::StyledPanel;
		opt.lineWidth = 1;

	    m_style->drawPrimitive(QStyle::PE_Frame, &opt, p);
	}
};
