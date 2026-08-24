#pragma once

#include <QCommonStyle>
#include <QPainter>

class KlassikStyle : public QCommonStyle
{
	Q_OBJECT
public:
	enum StyleType {
		Default,
		HighColor,
		B3
	};	  
	
	explicit KlassikStyle(StyleType type = Default);
	~KlassikStyle() override = default;

	void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt,
					   QPainter *p, const QWidget *widget = nullptr) const override;

	void drawControl(ControlElement control, const QStyleOption *opt,
					 QPainter *p, const QWidget *widget = nullptr) const override;

private:
	StyleType m_styleType = Default;

	void renderGradient(QPainter *p, const QRect &r, const QColor &color,
						bool horizontal = false) const;
};
