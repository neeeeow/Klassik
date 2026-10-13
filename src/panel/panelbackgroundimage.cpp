/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "panelbackgroundimage.h"

#include <QPainter>
#include <QTransform>
#include <QQmlFile>

PanelBackgroundImage::PanelBackgroundImage(QQuickItem *parent)
	: PaintedItem(parent)
{
	connect(this, &PaintedItem::propertyChanged, this, &PanelBackgroundImage::loadImage);
}

void
PanelBackgroundImage::componentComplete()
{
	loadImage();
	PaintedItem::componentComplete();
}

void
PanelBackgroundImage::paint(QPainter *p) const
{
	if (m_background.isNull())
		return;

	QImage backgroundTile;
	switch (m_location) {
	case Plasma::Types::LeftEdge: {
		QTransform transform;
		transform.rotate(90);			
		backgroundTile = m_background.transformed(transform).scaledToWidth(rect().width());
		break;
	}
	case Plasma::Types::TopEdge: {
		QTransform transform;
		transform.rotate(180);			
		backgroundTile = m_background.transformed(transform).scaledToHeight(rect().height());
		break;
	}
	case Plasma::Types::RightEdge: {
		QTransform transform;
		transform.rotate(270);			
		backgroundTile = m_background.transformed(transform).scaledToWidth(rect().width());
		break;
	}
	default: {
		backgroundTile = m_background.scaledToHeight(rect().height());
		break;
	}
	}
	
	p->save();
	p->setBrush(QBrush(backgroundTile));
	p->setPen(Qt::NoPen);
	p->drawRect(rect());		
	p->restore();
}

void
PanelBackgroundImage::loadImage()
{
	m_background = QImage();
	if (!m_source.isValid())
		return;
	m_background.load(QQmlFile::urlToLocalFileOrQrc(m_source));
	if (m_colorize)
		colorize(m_background);
}

void
PanelBackgroundImage::colorize(QImage &image)
{
	QColor color;
	
	const QPalette pal = QGuiApplication::palette();
	const QColor activeTitle = pal.color(QPalette::Active, QPalette::Highlight);
	const QColor inactiveTitle = pal.color(QPalette::Inactive, QPalette::Highlight);

	// figure out which color is most suitable for recoloring to
    int h1, s1, v1, h2, s2, v2, h3, s3, v3;
    activeTitle.getHsv(&h1, &s1, &v1);
    inactiveTitle.getHsv(&h2, &s2, &v2);
    pal.color(QPalette::Active, QPalette::Window).getHsv(&h3, &s3, &v3);

	if ( (qAbs(h1-h3)+qAbs(s1-s3)+qAbs(v1-v3) < qAbs(h2-h3)+qAbs(s2-s3)+qAbs(v2-v3)) &&
		 ((qAbs(h1-h3)+qAbs(s1-s3)+qAbs(v1-v3) < 32) || (s1 < 32)) && (s2 > s1))
		color = inactiveTitle;
	else
		color = activeTitle;

	int r, g, b;
	color.getRgb(&r, &g, &b);
	int gray = qGray(r, g, b);
	if (gray > 180) {
		r = (r - (gray - 180) < 0 ? 0 : r - (gray - 180));
		g = (g - (gray - 180) < 0 ? 0 : g - (gray - 180));
		b = (b - (gray - 180) < 0 ? 0 : b - (gray - 180));
	} else if (gray < 76) {
		r = (r + (76 - gray) > 255 ? 255 : r + (76 - gray));
		g = (g + (76 - gray) > 255 ? 255 : g + (76 - gray));
        b = (b + (76 - gray) > 255 ? 255 : b + (76 - gray));
	}
	color.setRgb(r, g, b);

	// convert the image, just in case
	if (image.format() != QImage::Format_ARGB32) {
		image = image.convertToFormat(QImage::Format_ARGB32);
	}

	int rval, gval, bval, val, alpha;
    float rcol = color.red(), gcol = color.green(), bcol = color.blue();

	for (int y = 0; y < image.height(); ++y) {
		QRgb *line = reinterpret_cast<QRgb*>(image.scanLine(y));
		for (int x = 0; x < image.width(); ++x) {
			val = qGray(line[x]);
			if (val < 128) {
				rval = static_cast<int>(rcol/128*val);
				gval = static_cast<int>(gcol/128*val);
				bval = static_cast<int>(bcol/128*val);
			}
			else if (val > 128) {
				rval = static_cast<int>((val-128)*(2-rcol/128)+rcol-1);
				gval = static_cast<int>((val-128)*(2-gcol/128)+gcol-1);
				bval = static_cast<int>((val-128)*(2-bcol/128)+bcol-1);
			}
			else { // val == 128
				rval = static_cast<int>(rcol);
				gval = static_cast<int>(gcol);
				bval = static_cast<int>(bcol);
			}

			alpha = qAlpha(line[x]);
			line[x] = qRgba(rval, gval, bval, alpha);
		}
	}
}
