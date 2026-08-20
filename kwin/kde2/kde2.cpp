#include "kde2.h"
#include "bits.h"

#include <KDecoration3/DecorationSettings>
#include <KDecoration3/DecoratedWindow>
#include <KPluginFactory>
#include <KIconLoader>

#include <QPainter>
#include <QBitmap>
#include <QTimer>

#include <qdrawutil.h>

#define TITLE_EDGE_TOP 3
#define TITLE_EDGE_BOTTOM 1

// Pixmaps used by buttons are declared globally
QPixmap pinDownPix;
QPixmap pinUpPix;

QPixmap leftBtnUpPix;
QPixmap leftBtnDownPix;
QPixmap ileftBtnUpPix;
QPixmap ileftBtnDownPix;

QPixmap rightBtnUpPix;
QPixmap rightBtnDownPix;
QPixmap irightBtnUpPix;
QPixmap irightBtnDownPix;

K_PLUGIN_FACTORY_WITH_JSON(
	KDE2DecorationFactory,
	"metadata.json",
	registerPlugin<KDE2Decoration>();
	)

static void
colorBitmaps(QPainter *p, const QPalette &palette, int x, int y, int w,
			 int h, bool isXBitmaps, const uchar *lightColor,
			 const uchar *midColor, const uchar *midlightColor,
			 const uchar *darkColor, const uchar *blackColor,
			 const uchar *whiteColor)
{

	const uchar *data[]={lightColor, midColor, midlightColor, darkColor,
		blackColor, whiteColor};
	
	QColor colors[]={palette.light().color(), palette.mid().color(), palette.midlight().color(),
		palette.dark().color(), Qt::black, Qt::white};

	int i;
	QBitmap b;
	for(i=0; i < 6; ++i){
		if(data[i]){
			b = QBitmap::fromData(QSize(w,h), data[i],
								  isXBitmaps ? QImage::Format_MonoLSB
								  : QImage::Format_Mono);
			b.setMask(b);
			p->setPen(colors[i]);
			p->drawPixmap(x, y, b);
		}
	}
}

static void
drawGradient(QPixmap &pixmap, const QColor &ca, const QColor &cb,
			 qreal x1=0, qreal y1=0, qreal x2=0, qreal y2=1)
{
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing);
	
	QLinearGradient gradient(x1, y1, x2, y2);
	gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
	gradient.setColorAt(0, ca);
	gradient.setColorAt(1, cb);
	
	painter.fillRect(pixmap.rect(), gradient);
	painter.end();
}

static QPixmap&
pixmapIntensity(QPixmap &pixmap, float percent)
{
	/* Reimplementation of KDE 3's KPixmapEffect/KImageEffect::intensity
	   Copyright (C) 1998, 1999, 2001, 2002 Daniel M. Duley <mosfet@kde.org>
	   (C) 1998, 1999 Christian Tibirna <ctibirna@total.net>
	   (C) 1998, 1999 Dirk Mueller <mueller@kde.org>
	   (C) 1999 Geert Jansen <g.t.jansen@stud.tue.nl>
	   (C) 2000 Josef Weidendorfer <weidendo@in.tum.de>
	   (C) 2004 Zack Rusin <zack@kde.org>
	*/

	QImage image = pixmap.toImage();

	int segColors = image.depth() > 8 ? 256 : image.colorCount();
	int pixels = image.depth() > 8 ? image.width() * image.height() :
		image.colorCount();
	unsigned int *data = image.depth() > 8 ? (unsigned int *)image.bits() :
		(unsigned int *)image.colorTable().data();
	
	bool brighten = (percent >= 0);
	if (percent < 0)
		percent = -percent;

	unsigned char *segTbl = new unsigned char[segColors];
	int tmp;
	if(brighten){ // keep overflow check out of loops
		for(int i=0; i < segColors; ++i){
			tmp = (int)(i*percent);
			if(tmp > 255)
				tmp = 255;
			segTbl[i] = tmp;
		}
	}
	else{
		for(int i=0; i < segColors; ++i){
			tmp = (int)(i*percent);
			if(tmp < 0)
				tmp = 0;
			segTbl[i] = tmp;
		}
	}

	if(brighten){ // same here
		for(int i=0; i < pixels; ++i){
			int r = qRed(data[i]);
			int g = qGreen(data[i]);
			int b = qBlue(data[i]);
			int a = qAlpha(data[i]);
			r = r + segTbl[r] > 255 ? 255 : r + segTbl[r];
			g = g + segTbl[g] > 255 ? 255 : g + segTbl[g];
			b = b + segTbl[b] > 255 ? 255 : b + segTbl[b];
			data[i] = qRgba(r, g, b,a);
		}
	}
	else{
		for(int i=0; i < pixels; ++i){
			int r = qRed(data[i]);
			int g = qGreen(data[i]);
			int b = qBlue(data[i]);
			int a = qAlpha(data[i]);
			r = r - segTbl[r] < 0 ? 0 : r - segTbl[r];
			g = g - segTbl[g] < 0 ? 0 : g - segTbl[g];
			b = b - segTbl[b] < 0 ? 0 : b - segTbl[b];
			data[i] = qRgba(r, g, b, a);
		}
	}
	delete [] segTbl;
	pixmap = QPixmap::fromImage(image);

	return pixmap;
}

static QRect
getScaledRect(const QRectF &rect, const qreal dpr)
{
	return QRect(qRound(rect.x() * dpr), qRound(rect.y() * dpr), qRound(rect.width() * dpr), qRound(rect.height() * dpr));
}

KDE2Decoration::KDE2Decoration(QObject *parent, const QVariantList &args) : Decoration(parent, args)
{
}

bool
KDE2Decoration::init()
{
	updateBorders();	
	createPixmaps();

	// Create buttons
	auto createGroup = [this](KDecoration3::DecorationButtonGroup::Position pos) {
		auto group = new KDecoration3::DecorationButtonGroup(pos, this, [pos](auto type, auto deco, auto parent) {
			return KDE2Button::create(type, pos, deco, parent);
		});
		group->setSpacing(0);
		return group;
	};
	m_leftButtons  = createGroup(KDecoration3::DecorationButtonGroup::Position::Left);
	m_rightButtons = createGroup(KDecoration3::DecorationButtonGroup::Position::Right);

	auto s = settings();

	/* Settings changes */
	// buttons
    connect(s.get(), &KDecoration3::DecorationSettings::decorationButtonsLeftChanged, this, &KDE2Decoration::updateButtonsGeometryDelayed);
	connect(s.get(), &KDecoration3::DecorationSettings::decorationButtonsRightChanged, this, &KDE2Decoration::updateButtonsGeometryDelayed);

	// full reconfiguration
	connect(s.get(), &KDecoration3::DecorationSettings::reconfigured, this, &KDE2Decoration::reconfigure);

	/* Window state changes */
	connect(window(), &KDecoration3::DecoratedWindow::paletteChanged, this, &KDE2Decoration::createPixmaps);
	
	// Update() signals
	connect(window(), &KDecoration3::DecoratedWindow::activeChanged, this, [this]() { update(); });

	// titleBar signals
	connect(window(), &KDecoration3::DecoratedWindow::captionChanged, this, [this]() {
		// update the caption area
		update(titleBar());
    });

	// Scale change
	connect(window(), &KDecoration3::DecoratedWindow::scaleChanged, this, &KDE2Decoration::reconfigure);

	// Border updates
	connect(s.get(), &KDecoration3::DecorationSettings::borderSizeChanged, this, &KDE2Decoration::updateBorders);
	connect(window(), &KDecoration3::DecoratedWindow::maximizedChanged, this, &KDE2Decoration::updateBorders);
	connect(this, &KDecoration3::Decoration::bordersChanged, this, &KDE2Decoration::updateButtonsGeometry);
	
	// Button signals. as a reminder: update() and updateTitleBar() is called in updateButtonsGeometry
	connect(window(), &KDecoration3::DecoratedWindow::sizeChanged, this, &KDE2Decoration::updateButtonsGeometry);
	connect(window(), &KDecoration3::DecoratedWindow::widthChanged, this, &KDE2Decoration::updateButtonsGeometry);
    connect(window(), &KDecoration3::DecoratedWindow::adjacentScreenEdgesChanged, this, &KDE2Decoration::updateButtonsGeometry);
    connect(window(), &KDecoration3::DecoratedWindow::shadedChanged, this, &KDE2Decoration::updateButtonsGeometry);

	updateButtonsGeometry();
	update();
	
	return true;
}

void
KDE2Decoration::reconfigure()
{
	/* This is called whenever the windows are reconfigured */

	updateBorders();
	createPixmaps();	
	updateButtonsGeometryDelayed();
}

void
KDE2Decoration::updateBorders()
{
	// Read in border size from settings
	{		
		auto s = settings();
		using enum KDecoration3::BorderSize;
		switch (s->borderSize()) {
		case Large:
			m_borderWidth = 8;
			break;
		case VeryLarge:
			m_borderWidth = 12;
			break;
		case Huge:
			m_borderWidth = 18;
			break;
		case VeryHuge:
			m_borderWidth = 27;
			break;
		case Oversized:
			m_borderWidth = 40;
			break;
		case Normal:
		default:
			m_borderWidth = 4;
		}
	}
	m_grabBorderWidth = (m_borderWidth > 15) ? m_borderWidth + 15 : 2*m_borderWidth;
	
	const qreal scale = window()->scale();
	QFontMetrics metrics(settings()->font());
	int topBorderWidth = std::max(14, metrics.height() + 2) + TITLE_EDGE_TOP + TITLE_EDGE_BOTTOM;
	m_titleHeight = topBorderWidth - ((TITLE_EDGE_TOP + TITLE_EDGE_BOTTOM)/scale);   
	
    setBorders(QMarginsF(m_borderWidth, topBorderWidth, m_borderWidth, window()->isMaximized() ? m_borderWidth : m_grabBorderWidth));
}

void
KDE2Decoration::updateButtonsGeometry()
{
	if (!m_leftButtons || !m_rightButtons)
		return;

	//TODO: set better geometry
	const qreal scale = window()->scale();
	const auto buttons = m_leftButtons->buttons() + m_rightButtons->buttons();
	for (auto *button : buttons) {
		if (button->type() == KDecoration3::DecorationButtonType::Spacer)
			button->setGeometry(QRectF(0, 0, 2/scale, m_titleHeight));
		else
			button->setGeometry(QRectF(0, 0, m_titleHeight, m_titleHeight));
	}

	m_leftButtons->setPos(QPointF(m_borderWidth, TITLE_EDGE_TOP/scale));
	m_rightButtons->setPos(QPointF(size().width() - m_rightButtons->geometry().width() - m_borderWidth, TITLE_EDGE_TOP/scale));
	
	updateTitleBar();
	update();
}

void
KDE2Decoration::updateTitleBar()
{
	int x, width;	
	const qreal scale = window()->scale();
	if (m_leftButtons && m_rightButtons) {
		x = m_leftButtons->geometry().right() + 1/scale;
		width = m_rightButtons->geometry().left() - 2/scale - x;
	} else {
		x = 0;
		width = window()->width();
	}

	setTitleBar(QRectF(x, TITLE_EDGE_TOP/scale, width, m_titleHeight));
}

void
KDE2Decoration::updateButtonsGeometryDelayed()
{
	QTimer::singleShot(0, this, &KDE2Decoration::updateButtonsGeometry);
}

void
KDE2Decoration::createPixmaps()
{
	QPalette palette = window()->palette();
	QColor activeTitleColor(window()->color(KDecoration3::ColorGroup::Active,
											KDecoration3::ColorRole::TitleBar));
	QColor inactiveTitleColor(window()->color(KDecoration3::ColorGroup::Inactive,
											  KDecoration3::ColorRole::TitleBar));
	QColor activeButtonColor(window()->color(QPalette::Active, QPalette::Button));
	QColor inactiveButtonColor(window()->color(QPalette::Inactive, QPalette::Button));
	const qreal scale = window()->scale();

	// Titlebar stipple
	if (true) { // TODO: add option to show/hide the stipple
		QPainter p;
		QPainter maskPainter;		
		int i, x, y;
		titlePix = QPixmap(132, qRound(m_titleHeight*scale)+2);
		titlePix.fill(Qt::transparent);
		QBitmap mask(132, qRound(m_titleHeight*scale)+2);
		mask.fill(Qt::color0);

		p.begin(&titlePix);
		maskPainter.begin(&mask);
		maskPainter.setPen(Qt::color1);

		for(i=0, y=2; i < 9; ++i, y+=4) {
			for(x=1; x <= 132; x+=3) {
				p.setPen(activeTitleColor.lighter(150));
				p.drawPoint(x, y);
				maskPainter.drawPoint(x, y);
				p.setPen(activeTitleColor.darker(150));
				p.drawPoint(x+1, y+1);
				maskPainter.drawPoint(x+1, y+1);
			}
		}
		maskPainter.end();
		p.end();
		titlePix.setMask(mask);
	} else {
		titlePix = QPixmap();
	}

	QPainter p;
	
	// Pins
	pinUpPix = QPixmap(14, 14);
	pinUpPix.fill(Qt::transparent);
	p.begin( &pinUpPix );
	colorBitmaps( &p, palette, 0, 0, 14, 14, true, pinup_white_bits,
				  pinup_gray_bits, NULL, NULL, pinup_dgray_bits, NULL );
	p.end();
	pinUpPix.setMask(QBitmap::fromData(
						 QSize(14, 14),
						 pinup_mask_bits, QImage::Format_MonoLSB));

	pinDownPix = QPixmap(14, 14);
	pinDownPix.fill(Qt::transparent);
	p.begin( &pinDownPix );
	colorBitmaps( &p, palette, 0, 0, 14, 14, true, pindown_white_bits,
				  pindown_gray_bits, NULL, NULL, pindown_dgray_bits, NULL );
	p.end();
	pinDownPix.setMask(QBitmap::fromData(
						   QSize(14, 14),
						   pindown_mask_bits, QImage::Format_MonoLSB));

	auto drawButtonBackground = [](QPixmap &pixmap, const QColor &bg, bool sunken) {
		QPainter p;
		int w = pixmap.width();
		int h = pixmap.height();
		int x2 = w-1;
		int y2 = h-1;

		// Colors
		QColor light = bg.lighter(150);
		QColor dark = bg.darker();
		QColor mid = bg.darker(150);

		// background
		drawGradient(pixmap, bg.lighter(130), bg.darker(130));

		p.begin(&pixmap);
		// outer frame
		p.setPen(mid);
		p.drawLine(0, 0, x2, 0);
		p.drawLine(0, 0, 0, y2);
		p.setPen(light);
		p.drawLine(x2, 0, x2, y2);
		p.drawLine(0, x2, y2, x2);
		p.setPen(dark);
		p.drawRect(1, 1, w-3, h-3);
		p.setPen(sunken ? mid : light);
		p.drawLine(2, 2, x2-2, 2);
		p.drawLine(2, 2, 2, y2-2);
		p.setPen(sunken ? light : mid);
		p.drawLine(x2-2, 2, x2-2, y2-2);
		p.drawLine(2, x2-2, y2-2, x2-2);
	};

	// Button backgrounds	
	QSize buttonPixSize(qRound(m_titleHeight * scale), qRound(m_titleHeight * scale));	
	leftBtnUpPix = QPixmap(buttonPixSize);
	leftBtnUpPix.fill(Qt::transparent);
    leftBtnDownPix = QPixmap(buttonPixSize);
	leftBtnDownPix.fill(Qt::transparent);
    ileftBtnUpPix = QPixmap(buttonPixSize);
	ileftBtnUpPix.fill(Qt::transparent);
    ileftBtnDownPix = QPixmap(buttonPixSize);
	ileftBtnDownPix.fill(Qt::transparent);

    rightBtnUpPix = QPixmap(buttonPixSize);
	rightBtnUpPix.fill(Qt::transparent);
    rightBtnDownPix = QPixmap(buttonPixSize);
	rightBtnDownPix.fill(Qt::transparent);
    irightBtnUpPix = QPixmap(buttonPixSize);
	irightBtnUpPix.fill(Qt::transparent);
    irightBtnDownPix = QPixmap(buttonPixSize);
	irightBtnDownPix.fill(Qt::transparent);
	
	drawButtonBackground( leftBtnUpPix, activeTitleColor, false );
	drawButtonBackground( leftBtnDownPix, activeTitleColor, true );

	drawButtonBackground( rightBtnUpPix, activeButtonColor, false );
	drawButtonBackground( rightBtnDownPix, activeButtonColor, true );

	drawButtonBackground( ileftBtnUpPix, inactiveTitleColor, false );
	drawButtonBackground( ileftBtnDownPix, inactiveTitleColor, true );

	drawButtonBackground( irightBtnUpPix, inactiveButtonColor, false );
	drawButtonBackground( irightBtnDownPix, inactiveButtonColor, true );
}

void
KDE2Decoration::paint(QPainter *p, const QRectF &repaintRegion)
{	
	int offset;

	// Colors
	QPalette palette = window()->palette();
	QColor titleColor(window()->color(window()->isActive() ? KDecoration3::ColorGroup::Active :
									  KDecoration3::ColorGroup::Inactive,
									  KDecoration3::ColorRole::TitleBar));
	QColor frameColor(window()->color(window()->isActive() ? KDecoration3::ColorGroup::Active :
									  KDecoration3::ColorGroup::Inactive,
									  KDecoration3::ColorRole::Frame));
	QColor foregroundColor(window()->color(window()->isActive() ? KDecoration3::ColorGroup::Active :
										   KDecoration3::ColorGroup::Inactive,
										   KDecoration3::ColorRole::Foreground));

	// Window scale
	const qreal scale = window()->scale();
	const int scaledTitleHeight = qRound(m_titleHeight * scale);
	const int scaledBorderWidth = qRound(m_borderWidth * scale);
	const int scaledGrabBorderWidth = qRound(m_grabBorderWidth * scale);

	// Scale our QPainter as appropriate
	p->setRenderHint(QPainter::Antialiasing, false);
	p->save();
	p->scale(1/scale, 1/scale);

	// Obtain widget bounds.
    QRect r(getScaledRect(rect(), scale));
    int x = r.x();
    int y = r.y();
    int x2 = r.width() - 1;
    int y2 = r.height() - 1;
    int w  = r.width();
    int h  = r.height();

	// Fill the background of the decoration with the window color, since
	// with HiDPI scaling small 1px gaps can sometimes appear due to qdrawutil.h's rounding
	p->fillRect(r.adjusted(-1,-1,-1,-1), palette.window());

	// Determine where to place the extended left titlebar
	int leftFrameStart = (h > 42) ? y+scaledTitleHeight+26 : y+scaledTitleHeight;

	// Determine where to make the titlebar color transition
	r = getScaledRect(titleBar(), scale);
	int rightOffset = r.x()+r.width()+1;

	// Create a disposable pixmap buffer for the titlebar
	// very early before drawing begins so there is no lag
	// during painting pixels.
    QPixmap titleBuffer( rightOffset-3, scaledTitleHeight + 1 );
	titleBuffer.fill(Qt::transparent);
	
	// Draw an outer black frame
	p->setPen(Qt::black);
	p->drawRect(x,y,w-1,h-1);

	// Draw part of the frame that is the titlebar color
	p->setPen(titleColor.lighter());
	p->drawLine(x+1, y+1, rightOffset-1, y+1);
	p->drawLine(x+1, y+1, x+1, leftFrameStart+scaledBorderWidth-4);

	// Draw titlebar colour separator line
	p->setPen(titleColor.darker());
	p->drawLine(rightOffset-1, y+1, rightOffset-1, scaledTitleHeight +2);

	p->fillRect(x+2, y+scaledTitleHeight+3,
	           scaledBorderWidth-4, leftFrameStart+scaledBorderWidth-y-scaledTitleHeight-8,
	           titleColor);

	
	// Finish drawing the titlebar extension
	p->setPen(Qt::black);
	p->drawLine(x+1, leftFrameStart+scaledBorderWidth-4, x+scaledBorderWidth-2, leftFrameStart-1);
	p->setPen(titleColor.darker(150));
	p->drawLine(x+scaledBorderWidth-2, y+scaledTitleHeight+3, x+scaledBorderWidth-2, leftFrameStart-2);
	
    // Fill out the border edges
    p->setPen(frameColor.lighter());
    p->drawLine(rightOffset, y+1, x2-1, y+1);
    p->drawLine(x+1, leftFrameStart+scaledBorderWidth-3, x+1, y2-1);
    p->setPen(frameColor.darker());
    p->drawLine(x2-1, y+1, x2-1, y2-1);
    p->drawLine(x+1, y2-1, x2-1, y2-1);

	p->setPen(frameColor);
	QPolygon a;
	QBrush brush( frameColor, Qt::SolidPattern );
	p->setBrush(brush);
	a.setPoints( 4, x+2,             leftFrameStart+scaledBorderWidth-4,
				 x+scaledBorderWidth-2, leftFrameStart,
				 x+scaledBorderWidth-2, y2-2,
				 x+2,             y2-2);
	p->drawPolygon(a);
	p->fillRect(x2-scaledBorderWidth+2, y+scaledTitleHeight+3,
				scaledBorderWidth-3, y2-y-scaledTitleHeight-4,
				frameColor);

	// Draw the bottom handle if required
	if (!window()->isMaximized()) {
		// We need to use non-scaled coordinates here, since qDrawShadePanel scales things internally, and as such
		// if we pre-scale the painter things get ugly.
		int x = rect().x();
		int x2 = rect().width() - 1;
		int y2 = rect().height() - 1;
		int w  = rect().width();
		p->scale(scale, scale);
		
		if (w > 50) {
			qDrawShadePanel(p, x+1, y2-m_grabBorderWidth+2, 2*m_borderWidth+12, m_grabBorderWidth-2,
							palette, false, 1, &palette.mid());
			qDrawShadePanel(p, x+2*m_borderWidth+13, y2-m_grabBorderWidth+2, w-4*m_borderWidth-26, m_grabBorderWidth-2,
							palette, false, 1, window()->isActive() ?
							&palette.window() :
							&palette.mid());
			qDrawShadePanel(p, x2-2*m_borderWidth-12, y2-m_grabBorderWidth+2, 2*m_borderWidth+12, m_grabBorderWidth-2,
							palette, false, 1, &palette.mid());
		} else
			qDrawShadePanel(p, x+1, y2-m_grabBorderWidth+2, w-2, m_grabBorderWidth-2,
							palette, false, 1, window()->isActive() ?
							&palette.window() :
							&palette.mid());
		offset = scaledGrabBorderWidth;
		p->scale(1/scale, 1/scale);
	} else {
		p->fillRect(x+2, y2-scaledBorderWidth+2, w-4, scaledBorderWidth-3,
					frameColor);
		offset = scaledBorderWidth;
	}

	// Draw a frame around the wrapped widget.
    p->setPen( frameColor.darker() );
    p->drawRect( x+scaledBorderWidth-1, y+scaledTitleHeight+3, w-2*scaledBorderWidth+1, h-scaledTitleHeight-offset-3 );
	
	// Fill with frame color behind RHS buttons
	p->fillRect( rightOffset, y+2, x2-rightOffset-1, scaledTitleHeight+1, frameColor);

	// Draw the title bar
	QPainter p2(&titleBuffer);

	// Draw the title bar background
	p2.fillRect(0, 0, rightOffset - 3, scaledTitleHeight + 1, titleColor);

	// Draw the title text on the pixmap
	QFont fnt = settings()->font();
	fnt.setPointSize(fnt.pointSize() * scale);
	p2.setFont(fnt);

	// Draw the titlebar stipple if active and available
	if (window()->isActive() && !titlePix.isNull()) {
		QFontMetrics fm(fnt);
		int captionWidth = fm.horizontalAdvance(window()->caption());
		if (window()->caption().isRightToLeft())
			p2.drawTiledPixmap(r.x(), 0, r.width()-captionWidth-4,
							   scaledTitleHeight+1, titlePix);
		else
			p2.drawTiledPixmap(r.x()+captionWidth+3, 0, r.width()-captionWidth-4,
							   scaledTitleHeight+1, titlePix);
	}

	p2.setPen(foregroundColor);
	p2.drawText(r.x(), 1, r.width()-1, r.height(),
				(window()->caption().isRightToLeft() ? Qt::AlignRight : Qt::AlignLeft) | Qt::AlignVCenter,
				window()->caption());
	p2.end();

	p->drawPixmap(rect().x()+2, rect().y()+2, titleBuffer);
	if (m_leftButtons)
		m_leftButtons->paint(p, repaintRegion);
	if (m_rightButtons)
		m_rightButtons->paint(p, repaintRegion);

	p->restore();
}

KDE2Button::KDE2Button(KDecoration3::DecorationButtonType type,
					   KDecoration3::DecorationButtonGroup::Position position,
					   KDecoration3::Decoration *decoration,
					   QObject *parent)
	: KDecoration3::DecorationButton(type, decoration, parent),
	  m_position(position)
{	
	setGeometry(QRectF(0,0,14,14)); // default to 14x14 as a backup

	using enum KDecoration3::DecorationButtonType;
	switch (type) {
	case Maximize:
		connect(decoration->window(), &KDecoration3::DecoratedWindow::maximizedChanged, this, &KDE2Button::setIconBits);
		break;
	case Shade:
		connect(decoration->window(), &KDecoration3::DecoratedWindow::shadedChanged, this, &KDE2Button::setIconBits);
		break;
	case KeepBelow:
		connect(decoration->window(), &KDecoration3::DecoratedWindow::keepBelowChanged, this, &KDE2Button::setIconBits);
		break;
	case KeepAbove:
		connect(decoration->window(), &KDecoration3::DecoratedWindow::keepAboveChanged, this, &KDE2Button::setIconBits);
		break;
	default:
		break;
	}

	setIconBits();
}

KDE2Button
*KDE2Button::create(KDecoration3::DecorationButtonType type,
					KDecoration3::DecorationButtonGroup::Position position,
					KDecoration3::Decoration *decoration,
					QObject *parent)
{
	if (auto d = qobject_cast<KDecoration3::Decoration *>(decoration)) {
		KDE2Button *b = new KDE2Button(type, position, d, parent);
		return b;
	} else
		return nullptr;	
}

void
KDE2Button::setIconBits()
{
	// Set decoration bitmap to be drawn
	using enum KDecoration3::DecorationButtonType;
	switch (type()) {
	case Minimize:
		iconBits = QBitmap::fromData(QSize(10,10), iconify_bits);
		break;
	case Maximize:
		iconBits = QBitmap::fromData(QSize(10,10), decoration()->window()->isMaximized() ? minmax_bits : maximize_bits);
		break;
	case Close:
		iconBits = QBitmap::fromData(QSize(10,10), close_bits);
		break;
	case ContextHelp:
		iconBits = QBitmap::fromData(QSize(10,10), question_bits);
		break;
	case Shade:
		iconBits = QBitmap::fromData(QSize(10,10), decoration()->window()->isShaded() ? shade_off_bits : shade_on_bits);
		break;
	case KeepBelow:
		iconBits = QBitmap::fromData(QSize(10,10), decoration()->window()->isKeepBelow() ? below_off_bits : below_on_bits);
		break;
	case KeepAbove:
		iconBits = QBitmap::fromData(QSize(10,10), decoration()->window()->isKeepAbove() ? above_off_bits : above_on_bits);
		break;
	default:
		iconBits = QBitmap();
		break;
	}
}

void
KDE2Button::paint(QPainter *p, const QRectF &repaintRegion)
{
	Q_UNUSED(repaintRegion);

	if (type() == KDecoration3::DecorationButtonType::Spacer)
		return;

	// Get scaled button geometry
	const qreal scale = decoration()->window()->scale();
	QRect geometryScaled = getScaledRect(geometry(), scale);
	int x = geometryScaled.x();
	int y = geometryScaled.y();
	int w  = geometryScaled.width();
	int h = geometryScaled.height();
	
	const bool active = decoration()->window()->isActive();
	if (!iconBits.isNull()) {
		// First draw the button background
		QPixmap btnbg;
		if (isLeft() )	{
			if (isPressed())
				btnbg = active ?
					leftBtnDownPix : ileftBtnDownPix;
			else
				btnbg = active ?
					leftBtnUpPix : ileftBtnUpPix;
		} else {
			if (isPressed())
				btnbg = active ?
					rightBtnDownPix : irightBtnDownPix;
			else
				btnbg = active ?
					rightBtnUpPix : irightBtnUpPix;
		}
		p->drawPixmap(x,y,btnbg);

		// Next draw the button icon
		bool darkDeco = qGray(decoration()->window()->color(
								  KDecoration3::ColorGroup::Active,
								  isLeft() ? KDecoration3::ColorRole::TitleBar : KDecoration3::ColorRole::Frame).rgb()) > 127;

		if (isHovered())
			p->setPen( darkDeco ? Qt::darkGray : Qt::lightGray );
		else
			p->setPen( darkDeco ? Qt::black : Qt::white );

		int xOff = x + (w-10)/2;
		int yOff = y + (h-10)/2;
		p->drawPixmap(isPressed() ? xOff+1: xOff, isPressed() ? yOff+1 : yOff, iconBits);
	} else {
		QPixmap btnpix;
		if (type() == KDecoration3::DecorationButtonType::OnAllDesktops) {
			btnpix = isChecked() ? pinDownPix : pinUpPix;
		} else {
			int iconSize = KIconLoader::global()->currentSize(KIconLoader::Small);
			btnpix = decoration()->window()->icon().pixmap(iconSize,iconSize);
		}

		if (isHovered())
			btnpix = pixmapIntensity(btnpix, 0.8);

		if (w < 16) {
		    btnpix.convertFromImage(btnpix.toImage().scaled(12, 12));
			p->drawPixmap(x,y,btnpix);
		} else {
			p->drawPixmap(x + w/2-8, y + h/2-8, btnpix);
		}		
	}
}

#include "kde2.moc"
