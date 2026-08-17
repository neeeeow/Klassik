#include "kde2.h"
#include "bits.h"

#include <KDecoration3/DecorationSettings>
#include <KDecoration3/DecoratedWindow>
#include <KPluginFactory>

#include <QPainter>
#include <QBitmap>
#include <QTimer>

#define TITLE_EDGE_TOP 3
#define TITLE_EDGE_BOTTOM 1

// Pixmaps used by buttons are declared globally
QPixmap pinDownPix;
QPixmap pinUpPix;
QPixmap ipinDownPix;
QPixmap ipinUpPix;

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

static QRect
getScaledRect(const QRectF &rect, const qreal dpr)
{
	return QRect(qRound(rect.x() * dpr), qRound(rect.y() * dpr), qRound(rect.width() * dpr), qRound(rect.height() * dpr));
}


KDE2Decoration::KDE2Decoration(QObject *parent, const QVariantList &args) : KDecoration3::Decoration(parent, args)
{
}

bool
KDE2Decoration::init()
{
	updateBorders();	
	createPixmaps();

	// Create buttons
	m_leftButtons = new KDecoration3::DecorationButtonGroup(KDecoration3::DecorationButtonGroup::Position::Left,
															this, &KDE2Button::create);
    m_rightButtons = new KDecoration3::DecorationButtonGroup(KDecoration3::DecorationButtonGroup::Position::Right,
															 this, &KDE2Button::create);
	m_leftButtons->setSpacing(2);
	m_rightButtons->setSpacing(2);

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

	// Add / remove borders when maximized state is changed
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
KDE2Decoration::paint(QPainter *p, const QRectF &repaintRegion)
{	
	Q_UNUSED(repaintRegion);

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

	// Obtain widget bounds.
    QRect r(getScaledRect(rect(), scale));
    int x = r.x();
    int y = r.y();
    int x2 = r.width() - 1;
    int y2 = r.height() - 1;
    int w  = r.width();
    int h  = r.height();

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
	// TODO: add the handle, just do the handleless case for now
	p->fillRect(x+2, y2-scaledBorderWidth+2, w-4, scaledBorderWidth-3,
				frameColor);
	offset = scaledBorderWidth;

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
	const qreal scale = window()->scale();
	QFontMetrics metrics(settings()->font());

	int topBorderWidth = std::max(14, metrics.height() + 3) + TITLE_EDGE_TOP + TITLE_EDGE_BOTTOM;
	m_titleHeight = topBorderWidth - ((TITLE_EDGE_TOP + TITLE_EDGE_BOTTOM)/scale);   
	
    setBorders(QMarginsF(m_borderWidth, topBorderWidth, m_borderWidth, m_borderWidth));
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
		button->setGeometry(QRectF(0, 0, m_titleHeight, m_titleHeight));
	}

	m_leftButtons->setPos(QPointF(4, TITLE_EDGE_TOP/scale));
	m_rightButtons->setPos(QPointF(size().width() - m_rightButtons->geometry().width() - 4, TITLE_EDGE_TOP/scale));
	
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
		width = m_rightButtons->geometry().left() - x;
	} else {
		x = 0;
		width = window()->width();
	}

	setTitleBar(QRectF(x, TITLE_EDGE_TOP, width, m_titleHeight));
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
				p.setPen(inactiveTitleColor.darker(150));
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
		p.drawRect(1, 1, w-2, h-2);
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


KDE2Button::KDE2Button(KDecoration3::DecorationButtonType type,
					   KDecoration3::Decoration *decoration,
					   QObject *parent)
	: KDecoration3::DecorationButton(type, decoration, parent)
{	
	setGeometry(QRectF(0,0,14,14)); // default to 14x14 as a backup

	setIconBits();
}

KDE2Button
*KDE2Button::create(KDecoration3::DecorationButtonType type,
						 KDecoration3::Decoration *decoration,
						 QObject *parent)
{
	if (auto d = qobject_cast<KDecoration3::Decoration *>(decoration)) {
		KDE2Button *b = new KDE2Button(type, d, parent);
		return b;
	} else
		return nullptr;	
}

void
KDE2Button::setIconBits()
{
	// Set decoration bitmap to be drawn
	switch (type()) {
	case KDecoration3::DecorationButtonType::Minimize:
		if (decoration()->window()->isMinimizeable())
			iconBits = QBitmap::fromData(QSize(10,10), iconify_bits);
		else
			iconBits = QBitmap();
		connect(decoration()->window(), &KDecoration3::DecoratedWindow::minimizeableChanged, this, &KDE2Button::setIconBits, Qt::UniqueConnection);
		break;
	case KDecoration3::DecorationButtonType::Maximize:
		if (decoration()->window()->isMaximizeable())
			iconBits = QBitmap::fromData(QSize(10,10), decoration()->window()->isMaximized() ? minmax_bits : maximize_bits);
		else
			iconBits = QBitmap();
		
		connect(decoration()->window(), &KDecoration3::DecoratedWindow::maximizedChanged, this, &KDE2Button::setIconBits, Qt::UniqueConnection);
		connect(decoration()->window(), &KDecoration3::DecoratedWindow::maximizeableChanged, this, &KDE2Button::setIconBits, Qt::UniqueConnection);
		break;
	case KDecoration3::DecorationButtonType::Close:
		if (decoration()->window()->isCloseable())
			iconBits = QBitmap::fromData(QSize(10,10), close_bits);
		else
			iconBits = QBitmap();
		connect(decoration()->window(), &KDecoration3::DecoratedWindow::closeableChanged, this, &KDE2Button::setIconBits, Qt::UniqueConnection);
		break;
	case KDecoration3::DecorationButtonType::ContextHelp:
		iconBits = QBitmap::fromData(QSize(10,10), question_bits);
		break;
	case KDecoration3::DecorationButtonType::Shade:
		if (decoration()->window()->isShadeable())
			iconBits = QBitmap::fromData(QSize(10,10), decoration()->window()->isShaded() ? shade_off_bits : shade_on_bits);
		else
			iconBits = QBitmap();

		connect(decoration()->window(), &KDecoration3::DecoratedWindow::shadedChanged, this, &KDE2Button::setIconBits, Qt::UniqueConnection);
		connect(decoration()->window(), &KDecoration3::DecoratedWindow::shadeableChanged, this, &KDE2Button::setIconBits, Qt::UniqueConnection);
		break;

	case KDecoration3::DecorationButtonType::KeepBelow:
		iconBits = QBitmap::fromData(QSize(10,10), decoration()->window()->isKeepBelow() ? below_off_bits : below_on_bits);
		connect(decoration()->window(), &KDecoration3::DecoratedWindow::keepBelowChanged, this, &KDE2Button::setIconBits, Qt::UniqueConnection);
		break;
	case KDecoration3::DecorationButtonType::KeepAbove:
		iconBits = QBitmap::fromData(QSize(10,10), decoration()->window()->isKeepAbove() ? above_off_bits : above_on_bits);
		connect(decoration()->window(), &KDecoration3::DecoratedWindow::keepAboveChanged, this, &KDE2Button::setIconBits, Qt::UniqueConnection);
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
}

#include "kde2.moc"
