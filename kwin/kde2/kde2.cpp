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
	
	return true;
}

void
KDE2Decoration::paint(QPainter *p, const QRectF &repaintRegion)
{	
	Q_UNUSED(repaintRegion);

	// Colors
	QPalette palette = window()->palette();
	QColor activeTitleColor(window()->color(KDecoration3::ColorGroup::Active,
											KDecoration3::ColorRole::TitleBar));
	QColor inactiveTitleColor(window()->color(KDecoration3::ColorGroup::Inactive,
											  KDecoration3::ColorRole::TitleBar));
	QColor activeButtonColor(window()->color(QPalette::Active, QPalette::Button));
	QColor inactiveButtonColor(window()->color(QPalette::Inactive, QPalette::Button));

	// Window scale
	const qreal scale = window()->scale();

	// Obtain widget bounds.
    QRect r(getScaledRect(rect(), scale));
    int x = r.x();
    int y = r.y();
    int x2 = r.width() - 1;
    int y2 = r.height() - 1;
    int w  = r.width();
    int h  = r.height();

	// Draw an outer black frame
	p->setPen(Qt::black);
	p->drawRect(x,y,w-1,h-1);
}

void
KDE2Decoration::updateBorders()
{
	const qreal scale = window()->scale();
	QFontMetrics metrics(settings()->font());

	int topBorderWidth = std::max(14, metrics.height() + 3) + TITLE_EDGE_TOP + TITLE_EDGE_BOTTOM;
	m_titleHeight = topBorderWidth - ((TITLE_EDGE_TOP + TITLE_EDGE_BOTTOM)/scale);
	
	int borderWidth = 4; // TODO: read border width from config	
	
    setBorders(QMarginsF(borderWidth, topBorderWidth, borderWidth, borderWidth));
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
    leftBtnDownPix = QPixmap(buttonPixSize);
    ileftBtnUpPix = QPixmap(buttonPixSize);
    ileftBtnDownPix = QPixmap(buttonPixSize);

    rightBtnUpPix = QPixmap(buttonPixSize);
    rightBtnDownPix = QPixmap(buttonPixSize);
    irightBtnUpPix = QPixmap(buttonPixSize);
    irightBtnDownPix = QPixmap(buttonPixSize);
	
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
KDE2Button::paint(QPainter *p, const QRectF &repaintRegion)
{
	Q_UNUSED(repaintRegion);
}

#include "kde2.moc"
