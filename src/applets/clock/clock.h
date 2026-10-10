/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "painteditem.h"

#include <QPainter>
#include <QColor>
#include <QDateTime>

class Clock : public PaintedItem
{
	Q_OBJECT
	QML_ANONYMOUS
	Q_PROPERTY(bool showFrame MEMBER m_showFrame NOTIFY propertyChanged)
	Q_PROPERTY(ColorTheme colorTheme MEMBER m_colorTheme NOTIFY propertyChanged)
	Q_PROPERTY(QColor fgColor MEMBER m_fgColor NOTIFY propertyChanged)
	Q_PROPERTY(QColor shadowColor MEMBER m_shadowColor NOTIFY propertyChanged)
	Q_PROPERTY(QColor bgColor MEMBER m_bgColor NOTIFY propertyChanged)
public:
	explicit Clock(QQuickItem *parent = nullptr);

	// Enum mirroring the color options in main.xml
	enum ColorTheme {
		System = 0,
		LCD = 1,
		Custom = 2
	};
	Q_ENUM(ColorTheme)

	// Functions for calculating the preferred clock geometry. Must
	// be implemented by subclasses
	Q_INVOKABLE virtual int preferredWidthForHeight(int h) const = 0;
	Q_INVOKABLE virtual int preferredHeightForWidth(int w) const = 0;

protected:
	// Clock properties
	bool m_showFrame = true;
	ColorTheme m_colorTheme = System;
	QColor m_fgColor = QColor("#000000");
	QColor m_shadowColor = QColor("#808080");
	QColor m_bgColor = QColor("#ffffff");

	// Colors for drawing the clock
	struct Colors {
		QColor fg;
		QColor shadow;
	};
	Colors getColors() const;
	
private:
	QPixmap m_lcdPixmap; // Pixmap for the LCD background

	void paint(QPainter *p) const override;

	// Function which draws the actual clock contents itself, and must
	// be overriden. The drawing of the background/frame are handled
	// by this base class, and do not need to be implemented in the
	// below function. This function should *never* be called directly
	// in any subclass
	virtual void drawContents(QPainter *p) const = 0;
};

class DigitalClock : public Clock
{
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(QString text MEMBER m_timeString NOTIFY propertyChanged)	
public:
	explicit DigitalClock(QQuickItem *parent = nullptr);
	
    int preferredWidthForHeight(int h) const override;
    int preferredHeightForWidth(int w) const override; 

private:
	QString m_timeString; // Contains the time we paint		
	
	// Drawing logic of QLCDNumber.cpp
	void drawContents(QPainter *p) const override;
	void drawString(const QString &s, const QRect &rect, const QColor &color, QPainter &p) const;
	void drawDigit(const QPoint &pos, const QColor &color, QPainter &p, int segLen,
				   char ch) const;
	void drawSegment(const QPoint &pos, const QColor &color, char segmentNo, QPainter &p,
					 int segLen) const;
};

class AnalogClock : public Clock
{
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(QDateTime dateTime MEMBER m_dateTime NOTIFY propertyChanged)
	Q_PROPERTY(bool showSeconds MEMBER m_showSeconds NOTIFY propertyChanged)
	Q_PROPERTY(bool antialiasing MEMBER m_antialiasing NOTIFY propertyChanged)
public:
	explicit AnalogClock(QQuickItem *parent = nullptr);

    int preferredWidthForHeight(int h) const override;
    int preferredHeightForWidth(int w) const override;
	
private:
	// Clock properties
	QDateTime m_dateTime = QDateTime::currentDateTime();
	bool m_showSeconds = false;
	bool m_antialiasing = false;
	
	void drawContents(QPainter *p) const override;
};
