/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "painteditem.h"

#include <QQuickPaintedItem>
#include <QQuickWindow>
#include <QPainter>

#include <KConfigPropertyMap>

class Clock : public PaintedItem
{
	Q_OBJECT
	Q_PROPERTY(KConfigPropertyMap* config READ config WRITE setConfig)
public:
	explicit Clock(QQuickItem *parent = nullptr);

	// Functions for getting/setting config map in QML
	KConfigPropertyMap* config() const { return m_config; }
	void setConfig(KConfigPropertyMap* config);

	// Functions for calculating the preferred clock geometry. Must
	// be implemented by subclasses
	Q_INVOKABLE virtual int preferredWidthForHeight(int h) const = 0;
	Q_INVOKABLE virtual int preferredHeightForWidth(int w) const = 0;	

protected:	
	// Config getter
	template <typename T>
	T getConfigValue(const char *key) const {
		const QVariant value = configValue(key);
		if (!value.canConvert<T>()) {
			qWarning() << "key" << key << "type mismatch!";
			return T();
		}
		return value.value<T>();				
	}

	// Helper functions
	inline QRect getRect() const { return QRect(0,0,width(),height()); }	
	static inline qreal getDpr(const QPainter *p) {
		return p->device() ? p->device()->devicePixelRatio() : 1.0;
	}
    static inline QRect getScaledRect(const QRect &rect, const qreal dpr) {
		return QRect(qRound(rect.x() * dpr), qRound(rect.y() * dpr), rect.width() * dpr, rect.height() * dpr);
	}
	
private:
	QPixmap m_lcdPixmap; // Pixmap for the LCD background
	KConfigPropertyMap *m_config = nullptr; // Applet config map

	QVariant configValue(const char *key) const;
	
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
	Q_PROPERTY(QString text READ text WRITE setText)	
public:
	explicit DigitalClock(QQuickItem *parent = nullptr);

	// Functions for getting/setting the internal time string in QML
	QString text() const { return m_timeString; }
	void setText(const QString &newText);
	
	Q_INVOKABLE int preferredWidthForHeight(int h) const override;
	Q_INVOKABLE int preferredHeightForWidth(int w) const override; 

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
public:
	explicit AnalogClock(QQuickItem *parent = nullptr);

	Q_INVOKABLE int preferredWidthForHeight(int h) const override;
	Q_INVOKABLE int preferredHeightForWidth(int w) const override;
	
private:	
	void drawContents(QPainter *p) const override;
};
