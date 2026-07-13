#pragma once

#include <QQuickPaintedItem>
#include <QQuickItem>
#include <QPalette>
#include <QApplication>
#include <QStyle>
#include <QStyleOptionFrame>

class PanelButtonBackground : public QQuickPaintedItem
{
	/*
	   This class draws a sunken KDE 3 style panel button background.
	   NOTE: the background here is always drawn as sunken, use the visible
	   QML property to set whether or not the background is sunken
	*/
	  
	Q_OBJECT
	QML_ELEMENT
public:
	PanelButtonBackground(QQuickItem *parent = nullptr) : QQuickPaintedItem(parent) {}
	~PanelButtonBackground() override = default;

private:
	void paint(QPainter *p) override {		
		QStyle *style = QApplication::style();
		if (!style)
			return;

		QStyleOptionFrame opt;
		opt.palette = QGuiApplication::palette();
		opt.rect = boundingRect().toRect();
		opt.state = QStyle::State_Enabled | QStyle::State_Sunken;
		opt.features = QStyleOptionFrame::None;
		opt.frameShape = QFrame::Panel;
		opt.lineWidth = 1;

		style->drawPrimitive(QStyle::PE_Frame, &opt, p);
	}

	bool event(QEvent *ev) override {
		if (ev->type() == QEvent::ApplicationPaletteChange || ev->type() == QEvent::PaletteChange)
			update();
	
		return QQuickPaintedItem::event(ev);
	}
};
