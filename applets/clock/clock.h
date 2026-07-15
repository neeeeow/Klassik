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

	void paint(QPainter *p) override;
	bool event(QEvent *ev) override;

protected:		
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

	// Function which draws the actual clock contents itself, and must
	// be overriden. The drawing of the background/frame are handled
	// by this base class, and do not need to be implemented in the
	// below function. This function should *never* be called directly
	// in any subclass
	virtual void drawContents(QPainter *p) = 0;
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
	
	// Drawing logic of QLCDNumber.cpp
	void drawContents(QPainter *p) override;
	void drawString(const QString &s, const QColor &color, QPainter &p);
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
