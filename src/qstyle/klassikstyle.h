/*
   SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>
 
   SPDX-FileCopyrightText: 2001-2002 Karol Szwed <gallium@kde.org>
   SPDX-FileCopyrightText: 2001-2002 Fredrik Höglund <fredrik@kde.org>
   SPDX-FileCopyrightText: 2001 Martijn Klingens <klingens@kde.org>
   SPDX-FileCopyrightText: 2000 Daniel M. Duley <mosfet@kde.org>
   SPDX-FileCopyrightText: 2000 Dirk Mueller <mueller@kde.org>
  
   SPDX-License-Identifier: GPL-3.0-or-later
*/

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

	void polish(QWidget *widget) override;

	void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt,
					   QPainter *p, const QWidget *widget = nullptr) const override;

	void drawControl(ControlElement control, const QStyleOption *opt,
					 QPainter *p, const QWidget *widget = nullptr) const override;
	
	QRect subElementRect(SubElement element, const QStyleOption *opt,
						 const QWidget *widget = nullptr) const override;

	void drawComplexControl(ComplexControl control, const QStyleOptionComplex *opt,
							QPainter *p, const QWidget *widget = nullptr) const override;

	int pixelMetric(PixelMetric metric, const QStyleOption *opt = nullptr,
					const QWidget *widget = nullptr) const override;

	QSize sizeFromContents(ContentsType contents, const QStyleOption *opt,
						   const QSize &contentsSize,
						   const QWidget *widget = nullptr) const override;

	int styleHint(StyleHint sh, const QStyleOption *opt = nullptr,
				  const QWidget *widget = nullptr,
				  QStyleHintReturn *hret = nullptr) const override;

private:
	StyleType m_styleType = Default;

	void renderGradient(QPainter *p, const QRect &r, const QColor &color,
						bool horizontal = false) const;
};
