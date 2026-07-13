#pragma once

#include <QQuickPaintedItem>
#include <QQuickWindow>
#include <QPainter>

#include <KConfigPropertyMap>

class Clock : public QQuickPaintedItem
{
	Q_OBJECT
	Q_PROPERTY(KConfigPropertyMap* config READ config WRITE setConfig);
public:
	Clock(QQuickItem *parent = nullptr);
	virtual ~Clock() = default;

	KConfigPropertyMap* config() const { return m_config; }
	void setConfig(KConfigPropertyMap* config);

	// Functions for calculating the preferred clock geometry.
	Q_INVOKABLE virtual int preferredWidthForHeight(int h) const = 0;
	Q_INVOKABLE virtual int preferredHeightForWidth(int w) const = 0;

protected:
	bool event(QEvent *ev) override;
	
	// Function which draws the actual clock contents itself, and must
	// be overriden. The drawing of the background/frame are handled
	// by this base class, and do not need to be implemented in the
	// below function.
	virtual void drawContents(QPainter *p) = 0;

	// Quickly fetch the device pixel ratio for a given QPainter
	inline qreal getDpr() {
		return window() ? window()->devicePixelRatio() : 1.0;
	}
	
	// Scale a QRect for HiDPI displays
	static inline QRect getScaledRect(const QRect &rect, const qreal dpr) {
		return QRect(qRound(rect.x() * dpr), qRound(rect.y() * dpr), rect.width() * dpr, rect.height() * dpr);
	}
	
	// Config getter
	template <typename T>
	T getConfigValue(const char *key) {
		if (!m_config)
			return T();	

		QVariant value = m_config->property(key);
		if (value.isNull() || !value.isValid()) {
			qWarning("AppletConfig: Key not found!");
			return T();
		}
		if (!value.canConvert<T>()) {
			qWarning("AppletConfig: Key type mismatch!");
			return T();
		}

		return value.value<T>();				
	}
	
private:
	QPixmap m_lcdPixmap; // Pixmap for the LCD background
	KConfigPropertyMap *m_config = nullptr; // Applet config map

	void paint(QPainter *p) override; // declare paint private since it should never be overriden
};

class DigitalClock : public Clock
{
	Q_OBJECT
	Q_PROPERTY(QString text READ text WRITE setText);	
	QML_ELEMENT
public:
	DigitalClock(QQuickItem *parent = nullptr);
	~DigitalClock() override = default;

	QString text() const { return m_timeString; }
	void setText(const QString &newText);

private:
	QString m_timeString; // Contains the time we paint

	Q_INVOKABLE int preferredWidthForHeight(int h) const override;
	Q_INVOKABLE int preferredHeightForWidth(int w) const override;
	
	void drawContents(QPainter *p) override;
	
	// Drawing logic of QLCDNumber.cpp
	void drawString(const QString &s, const QRect &r, const QColor &color, QPainter &p);
	void drawDigit(const QPoint &pos, const QColor &color, QPainter &p, int segLen,
				   char ch);
	void drawSegment(const QPoint &pos, const QColor &color, char segmentNo, QPainter &p,
					 int segLen);
};

class AnalogClock : public Clock
{
	Q_OBJECT
	QML_ELEMENT
public:
	AnalogClock(QQuickItem *parent = nullptr);
	~AnalogClock() override = default;

private:
	Q_INVOKABLE int preferredWidthForHeight(int h) const override;
	Q_INVOKABLE int preferredHeightForWidth(int w) const override;
	
	void drawContents(QPainter *p) override;
};
