#pragma once

#include <QQuickPaintedItem>
#include <QPainter>

class DigitalClock : public QQuickPaintedItem
{
	Q_OBJECT
	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged);
	QML_ELEMENT
public:
	DigitalClock(QQuickItem *parent = nullptr);
	~DigitalClock() override = default;

	void paint(QPainter *p) override;

	QString text() const { return m_timeString; }
	void setText(const QString &newText);

Q_SIGNALS:
	void textChanged();
	
private:
	QPixmap m_lcdPixmap; // Pixmap for the LCD background
	QString m_timeString; // Contains the time we paint
	
	// Drawing logic of QLCDNumber.cpp
	void drawString(const QString &s, const QRect &r, const QColor &color, QPainter &p);
	void drawDigit(const QPoint &pos, const QColor &color, QPainter &p, int segLen,
				   char ch);
	void drawSegment(const QPoint &pos, const QColor &color, char segmentNo, QPainter &p,
					 int segLen);
};
