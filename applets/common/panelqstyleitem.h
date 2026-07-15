#pragma once

#include <QQuickPaintedItem>
#include <QQuickItem>
#include <QApplication>
#include <QStyle>
#include <QPainter>

class PanelQStyleItem : public QQuickPaintedItem
{
	/*
	  Template class for QQuickPaintedItems which will go in our panel,
	  and use QStyle for drawing elements.
	*/
	  
	Q_OBJECT
public:
	PanelQStyleItem(QQuickItem *parent = nullptr) : QQuickPaintedItem(parent) {
		if (qApp)
			connect(qApp->style(), &QObject::destroyed, this, &PanelQStyleItem::styleChanged, Qt::UniqueConnection);
	}
	virtual ~PanelQStyleItem() = default;

	virtual void paint(QPainter *p) override = 0;
	bool event(QEvent *ev) override {
		if (ev->type() == QEvent::ApplicationPaletteChange || ev->type() == QEvent::PaletteChange)
			update();
	
		return QQuickPaintedItem::event(ev);
	}

private:	
	void styleChanged() {
		// we cannot simply connect to QEvent::StyleChange
		if (QCoreApplication::closingDown())
			return;
		if (qApp && qApp->style()) {
		    connect(qApp->style(), &QObject::destroyed, this, &PanelQStyleItem::styleChanged, Qt::UniqueConnection);
			update();
		}
	}
};
