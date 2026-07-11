#pragma once

#include <QQuickPaintedItem>
#include <QPainter>

#include <KConfigPropertyMap>

class DigitalClock : public QQuickPaintedItem
{
	Q_OBJECT
	Q_PROPERTY(QString text READ text WRITE setText);
	Q_PROPERTY(KConfigPropertyMap* config READ config WRITE setConfig);
	QML_ELEMENT
	public:
	DigitalClock(QQuickItem *parent = nullptr);
	~DigitalClock() override = default;

	QString text() const { return m_timeString; }
	void setText(const QString &newText);

	KConfigPropertyMap* config() const { return m_config; }
	void setConfig(KConfigPropertyMap* config);

	Q_INVOKABLE int preferredWidthForHeight(int h) const;
	Q_INVOKABLE int preferredHeightForWidth(int w) const;

	void paint(QPainter *p) override;

private:
	QPixmap m_lcdPixmap; // Pixmap for the LCD background
	QString m_timeString; // Contains the time we paint
	KConfigPropertyMap *m_config = nullptr; // Applet config map
	
	// Drawing logic of QLCDNumber.cpp
	void drawString(const QString &s, const QRect &r, const QColor &color, QPainter &p);
	void drawDigit(const QPoint &pos, const QColor &color, QPainter &p, int segLen,
				   char ch);
	void drawSegment(const QPoint &pos, const QColor &color, char segmentNo, QPainter &p,
					 int segLen);

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
};
