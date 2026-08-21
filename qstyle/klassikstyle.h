#pragma once

#include <QCommonStyle>
#include <QPainter>

class KlassikStyle : public QCommonStyle
{
	Q_OBJECT
public:
	KlassikStyle() = default;
	~KlassikStyle() override = default;

	void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt,
					   QPainter *p, const QWidget *widget = nullptr) const override;

private:
	static inline qreal getDpr(const QPainter *p) {		
		return p->device() ? p->device()->devicePixelRatio() : 1.0;
	}

	// Scales a QRect for drawing pixel-perfect lines on HiDPI displays
	static inline QRect getScaledRect(const QRect &rect, const qreal dpr) {
		return QRect(qRound(rect.x() * dpr), qRound(rect.y() * dpr), rect.width() * dpr, rect.height() * dpr);
	}
};
