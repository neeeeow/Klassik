/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "panelbackground.h"

#include <QStyleOptionFrame>
#include <QBrush>
#include <QTransform>

#include <KColorScheme>

PanelBackground::PanelBackground(QQuickItem *parent) : KlassikQStyleItem(parent)
{
}

void
PanelBackground::componentComplete()
{
	loadBackground();
	KlassikQStyleItem::componentComplete();
}

bool
PanelBackground::event(QEvent *event)
{
	if (event->type() == QEvent::ApplicationPaletteChange)
		loadBackground();
	return KlassikQStyleItem::event(event);
}

void
PanelBackground::paint(QPainter *p) const
{
	QRect r = QRect(0,0,width(),height());		
	QPalette pal = QGuiApplication::palette();

	// Tint the color scheme if needed. Note: we tint both the window & button color,
	// since we can't be 100% sure which colorscheme the active QStyle uses for drawing frames
	if (m_colorizePanel) {
		pal.setColor(QPalette::Window, tintColor(pal.window().color()));
		pal.setColor(QPalette::Button, tintColor(pal.button().color()));
	}

	// Draw the background
	p->fillRect(r, pal.window());
	if (m_useBackground && !m_background.isNull()) {
		QImage backgroundTile;
		switch (m_panelLocation) {
		case Plasma::Types::LeftEdge: {
			QTransform transform;
			transform.rotate(90);			
			backgroundTile = m_background.transformed(transform).scaledToWidth(width());
			break;
		}
		case Plasma::Types::TopEdge: {
			QTransform transform;
			transform.rotate(180);			
			backgroundTile = m_background.transformed(transform).scaledToHeight(height());
			break;
		}
		case Plasma::Types::RightEdge: {
			QTransform transform;
			transform.rotate(270);			
			backgroundTile = m_background.transformed(transform).scaledToWidth(width());
			break;
		}
		default: {
		    backgroundTile = m_background.scaledToHeight(height());
			break;
		}
		}

		p->save();
		p->setBrush(QBrush(backgroundTile));
		p->setPen(Qt::NoPen);
		p->drawRect(r);		
		p->restore();
	}

	// Draw the panel frame
	if (!style() || !m_drawFrame)
		return;		
	QStyleOptionFrame opt;
	opt.palette = pal;
	opt.rect = r;
	opt.state = QStyle::State_Enabled | QStyle::State_Raised;
	opt.frameShape = QFrame::StyledPanel;
	opt.lineWidth = 2;

	style()->drawPrimitive(QStyle::PE_Frame, &opt, p);
}

void
PanelBackground::loadBackground()
{
	m_background = QImage(); // reinitialise the QImage
	if (!m_useBackground)
		return;
	
    QString bg;
	if (m_useCustomBackground && m_customBackgroundUrl.isValid())
		bg = m_customBackgroundUrl.isLocalFile() ? m_customBackgroundUrl.toLocalFile() : m_customBackgroundUrl.toString();
	else
		bg = QStringLiteral(":/qt/qml/plasma/applet/com/github/neeeeow/klassik/panel/defaultBackground.png");
	
	m_background.load(bg);
	if (m_colorizePanel && !m_background.isNull())
		colorize(m_background);
}

void
PanelBackground::colorize(QImage &image) const
{
	QColor color;
	
	KColorScheme activeScheme(QPalette::Active, KColorScheme::Selection);
	QColor activeTitle = activeScheme.background().color();

	KColorScheme inactiveScheme(QPalette::Inactive, KColorScheme::Selection);
	QColor inactiveTitle = inactiveScheme.background().color();

	// figure out which color is most suitable for recoloring to
    int h1, s1, v1, h2, s2, v2, h3, s3, v3;
    activeTitle.getHsv(&h1, &s1, &v1);
    inactiveTitle.getHsv(&h2, &s2, &v2);
    QGuiApplication::palette().color(QPalette::Active, QPalette::Window).getHsv(&h3, &s3, &v3);

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

QColor
PanelBackground::tintColor(const QColor &base) const
{
	QColor color;
	
	KColorScheme activeScheme(QPalette::Active, KColorScheme::Selection);
	QColor activeTitle = activeScheme.background().color();

	KColorScheme inactiveScheme(QPalette::Inactive, KColorScheme::Selection);
	QColor inactiveTitle = inactiveScheme.background().color();

	// figure out which color is most suitable for recoloring to
	int h1, s1, v1, h2, s2, v2, h3, s3, v3;
	activeTitle.getHsv(&h1, &s1, &v1);
	inactiveTitle.getHsv(&h2, &s2, &v2);
	QGuiApplication::palette().color(QPalette::Active, QPalette::Window).getHsv(&h3, &s3, &v3);

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

    int val = qGray(base.rgb());
	int rval, gval, bval;
	float rcol = color.red(), gcol = color.green(), bcol = color.blue();

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

	return QColor(rval, gval, bval, base.alpha());
}

