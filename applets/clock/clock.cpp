#include "clock.h"

#include <QPalette>
#include <QColor>
#include <QDateTime>
#include <QPolygon>

#include <qdrawutil.h>

Clock::Clock(QQuickItem *parent)
	: KlassikPaintedItem(parent),
	  m_lcdPixmap(QStringLiteral(":/qt/qml/plasma/applet/com/github/neeeeow/klassik/clock/lcd.png"))
{
}

void
Clock::setConfig(KConfigPropertyMap *config)
{
	if (m_config != config) {
		m_config = config;
		polish();
	}
}

void
Clock::paint(QPainter *p) const
{
	QRect r = getRect();
	
	// background
	if (getConfigValue<bool>("lcdLook") && !m_lcdPixmap.isNull()) {
		p->save();
		const qreal dpr = getDpr(p);
		if (!qFuzzyCompare(dpr, qreal(1))) {
			const qreal inverseScale = qreal(1) / dpr;
			p->scale(inverseScale, inverseScale);
			p->drawTiledPixmap(getScaledRect(r, dpr), m_lcdPixmap);
		} else		
			p->drawTiledPixmap(r, m_lcdPixmap);
		p->restore();
	} else if (getConfigValue<bool>("useCustomColors")) {
		QColor bgColor = getConfigValue<QColor>("bgColor");
		p->fillRect(r, bgColor);
	}
	
	p->save();
	drawContents(p);
	p->restore();

	// frame
	if (getConfigValue<bool>("showFrame"))
		qDrawShadePanel(p, r, QGuiApplication::palette(), true);
}

DigitalClock::DigitalClock(QQuickItem *parent)
	: Clock(parent)	  
{
}


void
DigitalClock::setText(const QString &newText)
{
	if (m_timeString != newText) {
		m_timeString = newText;
		polish();
	}
}

int
DigitalClock::preferredWidthForHeight(int h) const
{
	if (h > 29) h = 29;
	if (h < 0) h = 0;
	return (m_timeString.length()*h*5/11)+2;
}

int
DigitalClock::preferredHeightForWidth(int w) const
{
	if (w < 0) w = 0;
	return((w / m_timeString.length() * 2) + 6);
}

void
DigitalClock::drawContents(QPainter *p) const
{	
	if (!m_timeString.isEmpty()) {
		QColor fgColor;
		QColor shadowColor;

		if (getConfigValue<bool>("lcdLook")) {
			fgColor = Qt::black;
			shadowColor = QColor(128, 128, 128);
		} else if (getConfigValue<bool>("useCustomColors")) {
			fgColor = getConfigValue<QColor>("fgColor");
			shadowColor = getConfigValue<QColor>("shadowColor");
		} else {
			QPalette pal = QGuiApplication::palette();
			fgColor = pal.color(QPalette::WindowText);
			shadowColor = pal.color(QPalette::Mid);
		}

		p->save();
		QRect clockRect;
		const qreal dpr = getDpr(p);
		if (!qFuzzyCompare(dpr, qreal(1))) {
			const qreal inverseScale = qreal(1) / dpr;
			p->scale(inverseScale, inverseScale);
			p->translate(0.5, 0.5);
			clockRect = getScaledRect(getRect(), dpr);
		} else
			clockRect = getRect();
		
		p->translate(1, 1);
		drawString(m_timeString, clockRect, shadowColor, *p);
		p->translate(-2, -2);
		drawString(m_timeString, clockRect, fgColor, *p);

		p->restore();
	}
}

static const char
*getSegments(char ch)               // gets list of segments for ch
{
    static const char segments[30][8] =
       { { 0, 1, 2, 4, 5, 6,99, 0},             // 0    0 / O
         { 2, 5,99, 0, 0, 0, 0, 0},             // 1    1
         { 0, 2, 3, 4, 6,99, 0, 0},             // 2    2
         { 0, 2, 3, 5, 6,99, 0, 0},             // 3    3
         { 1, 2, 3, 5,99, 0, 0, 0},             // 4    4
         { 0, 1, 3, 5, 6,99, 0, 0},             // 5    5 / S
         { 0, 1, 3, 4, 5, 6,99, 0},             // 6    6
         { 0, 2, 5,99, 0, 0, 0, 0},             // 7    7
         { 0, 1, 2, 3, 4, 5, 6,99},             // 8    8
         { 0, 1, 2, 3, 5, 6,99, 0},             // 9    9 / g
         { 3,99, 0, 0, 0, 0, 0, 0},             // 10   -
         { 7,99, 0, 0, 0, 0, 0, 0},             // 11   .
         { 0, 1, 2, 3, 4, 5,99, 0},             // 12   A
         { 1, 3, 4, 5, 6,99, 0, 0},             // 13   B
         { 0, 1, 4, 6,99, 0, 0, 0},             // 14   C
         { 2, 3, 4, 5, 6,99, 0, 0},             // 15   D
         { 0, 1, 3, 4, 6,99, 0, 0},             // 16   E
         { 0, 1, 3, 4,99, 0, 0, 0},             // 17   F
         { 1, 3, 4, 5,99, 0, 0, 0},             // 18   h
         { 1, 2, 3, 4, 5,99, 0, 0},             // 19   H
         { 1, 4, 6,99, 0, 0, 0, 0},             // 20   L
         { 3, 4, 5, 6,99, 0, 0, 0},             // 21   o
         { 0, 1, 2, 3, 4,99, 0, 0},             // 22   P
         { 3, 4,99, 0, 0, 0, 0, 0},             // 23   r
         { 4, 5, 6,99, 0, 0, 0, 0},             // 24   u
         { 1, 2, 4, 5, 6,99, 0, 0},             // 25   U
         { 1, 2, 3, 5, 6,99, 0, 0},             // 26   Y
         { 8, 9,99, 0, 0, 0, 0, 0},             // 27   :
         { 0, 1, 2, 3,99, 0, 0, 0},             // 28   '
         {99, 0, 0, 0, 0, 0, 0, 0} };           // 29   empty

    if (ch >= '0' && ch <= '9')
        return segments[ch - '0'];
    if (ch >= 'A' && ch <= 'F')
        return segments[ch - 'A' + 12];
    if (ch >= 'a' && ch <= 'f')
        return segments[ch - 'a' + 12];

    int n;
    switch (ch) {
        case '-':
            n = 10;  break;
        case 'O':
            n = 0;   break;
        case 'g':
            n = 9;   break;
        case '.':
            n = 11;  break;
        case 'h':
            n = 18;  break;
        case 'H':
            n = 19;  break;
        case 'l':
        case 'L':
            n = 20;  break;
        case 'o':
            n = 21;  break;
        case 'p':
        case 'P':
            n = 22;  break;
        case 'r':
        case 'R':
            n = 23;  break;
        case 's':
        case 'S':
            n = 5;   break;
        case 'u':
            n = 24;  break;
        case 'U':
            n = 25;  break;
        case 'y':
        case 'Y':
            n = 26;  break;
        case ':':
            n = 27;  break;
        case '\'':
            n = 28;  break;
        default:
            n = 29;  break;
    }
    return segments[n];
}

void
DigitalClock::drawString(const QString &s, const QRect &rect, const QColor &color, QPainter &p) const
{
	if (s.isEmpty())
		return;

	int ndigits = s.length();
	int digitSpace = 1; // we make smallPoint *always* false in our case
	int xSegLen    = rect.width()*5/(ndigits*(5 + digitSpace) + digitSpace);
	int ySegLen    = rect.height()*5/12;
	int segLen     = ySegLen > xSegLen ? xSegLen : ySegLen;
	int xAdvance   = segLen*(5 + digitSpace)/5;
	int xOffset    = (rect.width() - ndigits*xAdvance + segLen/5)/2;
	int yOffset    = (rect.height() - segLen*2)/2;

	for (int i=0; i<ndigits; i++) {
		QPoint pos(xOffset + xAdvance*i, yOffset);
		drawDigit(pos, color, p, segLen, s[i].toLatin1());
	}
}


void
DigitalClock::drawDigit(const QPoint &pos, const QColor &color, QPainter &p, int segLen,
						char ch) const
{
	// Compared to the original drawDigit, we're doing a clean redraw *every* time
	const char *segs = getSegments(ch);

	for (int i = 0; segs[i] != 99; i++) {
		drawSegment(pos, color, segs[i], p, segLen);
	}
}



static void
addPoint(QPolygon &a, const QPoint &p)
{
	uint n = a.size();
	a.resize(n + 1);
	a.setPoint(n, p);
}

void
DigitalClock::drawSegment(const QPoint &pos, const QColor &color, char segmentNo,
						  QPainter &p, int segLen) const
{
    QPoint ppt;
    QPoint pt = pos;
    int width = segLen/5; 

#define LINETO(X,Y) addPoint(a, QPoint(pt.x() + (X),pt.y() + (Y)))
#define LIGHT
#define DARK

	QPolygon a(0); // we always want to fill, and we don't put the shadow here either.
	//The following is an exact copy of the switch below.
	//don't make any changes here
	switch (segmentNo) {
	case 0 :
		ppt = pt;
		LIGHT;
		LINETO(segLen - 1,0);
		DARK;
		LINETO(segLen - width - 1,width);
		LINETO(width,width);
		LINETO(0,0);
		break;
	case 1 :
		pt += QPoint(0 , 1);
		ppt = pt;
		LIGHT;
		LINETO(width,width);
		DARK;
		LINETO(width,segLen - width/2 - 2);
		LINETO(0,segLen - 2);
		LIGHT;
		LINETO(0,0);
		break;
	case 2 :
		pt += QPoint(segLen - 1 , 1);
		ppt = pt;
		DARK;
		LINETO(0,segLen - 2);
		LINETO(-width,segLen - width/2 - 2);
		LIGHT;
		LINETO(-width,width);
		LINETO(0,0);
		break;
	case 3 :
		pt += QPoint(0 , segLen);
		ppt = pt;
		LIGHT;
		LINETO(width,-width/2);
		LINETO(segLen - width - 1,-width/2);
		LINETO(segLen - 1,0);
		DARK;
		if (width & 1) {            // adjust for integer division error
			LINETO(segLen - width - 3,width/2 + 1);
			LINETO(width + 2,width/2 + 1);
		} else {
			LINETO(segLen - width - 1,width/2);
			LINETO(width,width/2);
		}
		LINETO(0,0);
		break;
	case 4 :
		pt += QPoint(0 , segLen + 1);
		ppt = pt;
		LIGHT;
		LINETO(width,width/2);
		DARK;
		LINETO(width,segLen - width - 2);
		LINETO(0,segLen - 2);
		LIGHT;
		LINETO(0,0);
		break;
	case 5 :
		pt += QPoint(segLen - 1 , segLen + 1);
		ppt = pt;
		DARK;
		LINETO(0,segLen - 2);
		LINETO(-width,segLen - width - 2);
		LIGHT;
		LINETO(-width,width/2);
		LINETO(0,0);
		break;
	case 6 :
		pt += QPoint(0 , segLen*2);
		ppt = pt;
		LIGHT;
		LINETO(width,-width);
		LINETO(segLen - width - 1,-width);
		LINETO(segLen - 1,0);
		DARK;
		LINETO(0,0);
		break;
	case 7 :
		pt += QPoint(segLen/2 , segLen*2);
		ppt = pt;
		DARK;
		LINETO(width,0);
		LINETO(width,-width);
		LIGHT;
		LINETO(0,-width);
		LINETO(0,0);
		break;
	case 8 :
		pt += QPoint(segLen/2 - width/2 + 1 , segLen/2 + width);
		ppt = pt;
		DARK;
		LINETO(width,0);
		LINETO(width,-width);
		LIGHT;
		LINETO(0,-width);
		LINETO(0,0);
		break;
	case 9 :
		pt += QPoint(segLen/2 - width/2 + 1 , 3*segLen/2 + width);
		ppt = pt;
		DARK;
		LINETO(width,0);
		LINETO(width,-width);
		LIGHT;
		LINETO(0,-width);
		LINETO(0,0);
		break;
	default :
		qWarning("DigitalClock::drawSegment: illegal segment!"); 
	}
	// End exact copy

	QPen pen(color);
	pen.setCosmetic(true);
	p.setPen(pen);	
	p.setBrush(color);
	p.drawPolygon(a);
	p.setBrush(Qt::NoBrush);
	p.setPen(Qt::NoPen);

	pt = pos;
#undef LINETO
#undef LIGHT
#undef DARK	
}

AnalogClock::AnalogClock(QQuickItem *parent)
	: Clock(parent)	  
{
	
}

int
AnalogClock::preferredWidthForHeight(int h) const
{
	return h;
}

int
AnalogClock::preferredHeightForWidth(int w) const
{
	return w;
}

void
AnalogClock::drawContents(QPainter *p) const
{	
	QTime time = QDateTime::currentDateTime().time();

	p->setRenderHint(QPainter::Antialiasing, getConfigValue<bool>("antialiasing"));

	QColor fgColor;
	QColor shadowColor;
	if (getConfigValue<bool>("lcdLook")) {
		fgColor = Qt::black;
		shadowColor = QColor(128, 128, 128);
	} else if (getConfigValue<bool>("useCustomColors")) {
		fgColor = getConfigValue<QColor>("fgColor");
		shadowColor = getConfigValue<QColor>("shadowColor");
	} else {
		QPalette pal = QGuiApplication::palette();
		fgColor = pal.color(QPalette::WindowText);
		shadowColor = pal.color(QPalette::Mid);
	}

	const qreal dpr = getDpr(p);
	int spWidth = width();
	int spHeight = height();	   

	QPolygon pts;
    QPoint cp(spWidth / 2, spHeight / 2);
	int d = qMin(spWidth,spHeight) - 10;

	QPen shadowPen(shadowColor);
	shadowPen.setCosmetic(true);
	p->setPen(shadowPen);
	p->setBrush(shadowColor);
    int offset = 2;
	
	for ( int c=0 ; c < 2 ; c++ ) {
		QTransform matrix; // keep the variable name matrix for convenience
        matrix.translate(cp.x() + offset, cp.y() + offset);
        matrix.scale( d/1000.0F, d/1000.0F );

		// hour
        float h_angle = 30*(time.hour()%12-3) + time.minute()/2;
        matrix.rotate( h_angle );
        p->setTransform( matrix );
        pts.setPoints( 4, -20,0,  0,-20, 300,0, 0,20 );
        p->drawPolygon( pts );
        matrix.rotate( -h_angle );

		// minute
        float m_angle = (time.minute()-15)*6;
        matrix.rotate( m_angle );
        p->setTransform( matrix );
        pts.setPoints( 4, -10,0, 0,-10, 400,0, 0,10 );
        p->drawPolygon( pts );
        matrix.rotate( -m_angle );

		if (getConfigValue<bool>("showSeconds")) {   // second
            float s_angle = (time.second()-15)*6;
            matrix.rotate( s_angle );
            p->setTransform( matrix );
            pts.setPoints(4,0,0,0,0,400,0,0,0);
            p->drawPolygon( pts );
            matrix.rotate( -s_angle );
        }

		QTransform matrix2;
        matrix2.translate(cp.x() + offset, cp.y() + offset);
        matrix2.scale( d/1000.0F, d/1000.0F );

        // quadrante
        for ( int i=0 ; i < 12 ; i++ ) {
            p->setTransform( matrix2 );
            p->drawLine( 460,0, 500,0 ); // draw hour lines
            matrix2.rotate( 30 );
        }

		QPen fgPen(fgColor);
		fgPen.setCosmetic(true);
		p->setPen(fgPen);
		p->setBrush(fgColor);

		offset = 0;
	}	
}
