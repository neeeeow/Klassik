#pragma once

#include "klassikpainteditem.h"

#include <QPalette>

#include <qdrawutil.h>

class SunkenAppletFrame : public KlassikPaintedItem
{
	/*
	   This class draws a sunken frame, using qDrawShadePanel. Typically used
	   for a sunken border effect on panel applets.
	*/
	  
	Q_OBJECT
	QML_ELEMENT
public:
	SunkenAppletFrame(QQuickItem *parent = nullptr) : KlassikPaintedItem(parent) {}
	~SunkenAppletFrame() override = default;

private:
	void paint(QPainter *p) const override {
		qDrawShadePanel(p, QRect(0,0,width(),height()), QGuiApplication::palette(), true);
	}
};
