/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "../common/klassikqstyleitem.h"

#include <QPalette>

#include <Plasma/Plasma>

class HandleArrow : public KlassikQStyleItem
{
	/*
	  This class draws the arrow used in the panel handle
	*/
	  
	Q_OBJECT
	QML_ELEMENT
    Q_PROPERTY(Plasma::Types::Location panelLocation READ panelLocation WRITE setPanelLocation)
public:
    HandleArrow(QQuickItem *parent = nullptr) : KlassikQStyleItem(parent) {}
    ~HandleArrow() override = default;

    Plasma::Types::Location panelLocation() const { return m_panelLocation; }
	void setPanelLocation(Plasma::Types::Location location) {
		if (m_panelLocation != location) {
			m_panelLocation = location;
			updateImage();
		}
	}
protected:	
	void paint(QPainter *p) const override {		
		if (!m_style)
			return;

		QStyleOption opt;
		opt.palette = QGuiApplication::palette();
		opt.rect = QRect(0,0,width(),height());
		opt.state = QStyle::State_Enabled;

		QStyle::PrimitiveElement pe;
		switch (m_panelLocation) {
		case Plasma::Types::LeftEdge:
			pe = QStyle::PE_IndicatorArrowRight;
			break;	 
		case Plasma::Types::TopEdge:
			pe = QStyle::PE_IndicatorArrowDown;
			break;
		case Plasma::Types::RightEdge:
			pe = QStyle::PE_IndicatorArrowLeft;
			break;
		default:
			pe = QStyle::PE_IndicatorArrowUp;
			break;
		}
		
		m_style->drawPrimitive(pe, &opt, p);
	}

private:
    Plasma::Types::Location m_panelLocation = Plasma::Types::BottomEdge;
};
