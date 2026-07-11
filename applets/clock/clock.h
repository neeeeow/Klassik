#pragma once

#include <QQuickPaintedItem>
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

	void paint(QPainter *p) override;

	// Functions for calculating the preferred clock geometry.
	Q_INVOKABLE virtual int preferredWidthForHeight(int h) const = 0;
	Q_INVOKABLE virtual int preferredHeightForWidth(int w) const = 0;

protected:
	// Function which draws the actual clock contents itself, and must
	// be overriden. The drawing of the background/frame are handled
	// by this base class, and do not need to be implemented in the
	// below function.
	virtual void drawContents(QPainter *p) = 0;
	
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
