#pragma once

#include "panelqstyleitem.h"

#include <QPalette>
#include <QStyleOption>

class PanelButtonArrow : public PanelQStyleItem
{
	/*
	   This class draws a sunken KDE 3 style panel button background.
	   NOTE: the background here is always drawn as sunken, use the visible
	   QML property to set whether or not the background is sunken
	*/
	  
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(bool active READ active WRITE setActive);
public:
	PanelButtonArrow(QQuickItem *parent = nullptr) : PanelQStyleItem(parent) {}
	~PanelButtonArrow() override = default;

	enum MenuLocation {
		Above,
		Below,
		Right,
		Left
	};

	bool active() const { return m_active; }
	void setActive(bool state) {
		if (m_active != state) {
			m_active = state;
			update();
		}
	}

	void paint(QPainter *p) override {		
		QStyle *style = QApplication::style();
		if (!style)
			return;  
		
		// Here we use the slightly older KDE 3 panel button arrow logic, it
		// looks better than the new style adopted later in 3.5's life.
		QStyle::PrimitiveElement e = QStyle::PE_IndicatorArrowUp;
		int arrowSize = style->pixelMetric(QStyle::PM_MenuButtonIndicator);
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
		style->drawPrimitive(e, &opt, p);
	}

private:
	MenuLocation m_location = Above;
	bool m_active = false;	
};
