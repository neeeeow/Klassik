#pragma once

#include <QQuickPaintedItem>
#include <QQuickItem>
#include <QPalette>

#include <qdrawutil.h>

class SunkenAppletFrame : public QQuickPaintedItem
{
	/*
	   This class draws a sunken frame, using qDrawShadePanel. Typically used
	   for a sunken border effect on panel applets.
	*/
	  
	Q_OBJECT
	QML_ELEMENT
public:
	SunkenAppletFrame(QQuickItem *parent = nullptr) : QQuickPaintedItem(parent) {}
	~SunkenAppletFrame() override = default;

private:
	void paint(QPainter *p) override {
		qDrawShadePanel(p, boundingRect().toRect(), QGuiApplication::palette(), true);
	}

	bool event(QEvent *ev) override {
		if (ev->type() == QEvent::ApplicationPaletteChange || ev->type() == QEvent::PaletteChange)
			update();
	
		return QQuickPaintedItem::event(ev);
	}
};
