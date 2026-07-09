#pragma once

#include <QQuickPaintedItem>
#include <QPainter>

class DigitalClock : public QQuickPaintedItem
{
	Q_OBJECT
	QML_ELEMENT
public:
	DigitalClock(QQuickItem *parent = nullptr);
	~DigitalClock() override = default;

	void paint(QPainter *p) override;

private:

	// Drawing logic of QLCDNumber.cpp
	void drawString(const QString &s, QPainter &p);
	void drawDigit(const QPoint &pos, QPainter &p, int segLen,
				   char ch);
	void drawSegment(const QPoint &pos, char segmentNo, QPainter &p,
					 int segLen);
};
