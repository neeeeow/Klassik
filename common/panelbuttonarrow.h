/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "klassikqstyleitem.h"

#include <QPalette>

class PanelButtonArrow : public KlassikQStyleItem
{
	/*
	   This class draws a sunken KDE 3 style popup menu arrow for buttons
	*/
	  
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(bool active READ active WRITE setActive)
	Q_PROPERTY(MenuLocation location READ location WRITE setLocation)
public:
	PanelButtonArrow(QQuickItem *parent = nullptr) : KlassikQStyleItem(parent) {}
	~PanelButtonArrow() override = default;

	enum MenuLocation {
		Above,
		Below,
		Right,
		Left
	};
	Q_ENUM(MenuLocation)

	bool active() const { return m_active; }
	void setActive(bool state) {
		if (m_active != state) {
			m_active = state;
			updateImage();
		}
	}

	MenuLocation location() const { return m_location; }
	void setLocation(MenuLocation location) {
		if (m_location != location) {
			m_location = location;
			updateImage();
		}
	}

protected:
	void paint(QPainter *p) const override {		
		if (!m_style)
			return;  
		
		// Here we use the slightly older KDE 3 panel button arrow logic, it
		// looks better than the new style adopted later in 3.5's life.
		QStyle::PrimitiveElement e = QStyle::PE_IndicatorArrowUp;
		int arrowSize = m_style->pixelMetric(QStyle::PM_MenuButtonIndicator);
		QRect r(0, 0, arrowSize, arrowSize);
		switch (m_location) {
		case Above:
			e = QStyle::PE_IndicatorArrowUp;
			break;
		case Below:
			e = QStyle::PE_IndicatorArrowDown;
			r.translate(0, height() - arrowSize);
			break;
		case Right:
			e = QStyle::PE_IndicatorArrowRight;
		    r.translate(width() - arrowSize, 0);
			break;
		case Left:
			e = QStyle::PE_IndicatorArrowLeft;
			break;
		}

		QStyleOption opt;
		opt.rect = r;
		opt.palette = QGuiApplication::palette();
		opt.state = QStyle::State_Enabled;
		if (m_active)
			opt.state |= QStyle::State_Sunken; // most QStyle's don't have a separate sunken state, but just in case
		m_style->drawPrimitive(e, &opt, p);
	}

private:
	MenuLocation m_location = Above;
	bool m_active = false;	
};
