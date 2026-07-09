#include "digitalclock.h"

#include <QPalette>
#include <QQuickWindow>

DigitalClock::DigitalClock(QQuickItem *parent) : QQuickPaintedItem(parent)
{
}

void
DigitalClock::paint(QPainter *p)
{
	p->setRenderHint(QPainter::Antialiasing, false);
	drawString(QStringLiteral("12:34"), *p);
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
DigitalClock::drawString(const QString &s, QPainter &p)
{
	if (s.isEmpty())
		return;

	int ndigits = s.length();
	int digitSpace = 1; // we make smallPoint *always* false in our case
	int xSegLen    = boundingRect().width()*5/(ndigits*(5 + digitSpace) + digitSpace);
	int ySegLen    = boundingRect().height()*5/12;
	int segLen     = ySegLen > xSegLen ? xSegLen : ySegLen;
	int xAdvance   = segLen*(5 + digitSpace)/5;
	int xOffset    = (boundingRect().width() - ndigits*xAdvance + segLen/5)/2;
	int yOffset    = (boundingRect().height() - segLen*2)/2;

	for (int i=0; i<ndigits; i++) {
		QPoint pos(xOffset + xAdvance*i, yOffset);
		drawDigit(pos, p, segLen, s[i].toLatin1());
	}
}


void
DigitalClock::drawDigit(const QPoint &pos, QPainter &p, int segLen,
						char ch)
{
	// Compared to the original drawDigit, we're doing a clean redraw *every* time
	const char *segs = getSegments(ch);

	for (int i = 0; segs[i] != 99; i++) {
		drawSegment(pos, segs[i], p, segLen);
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
DigitalClock::drawSegment(const QPoint &pos, char segmentNo, QPainter &p,
                                    int segLen)
{
    QPoint ppt;
    QPoint pt = pos;
    int width = segLen/5;

    //const QPalette &pal = q->palette();
	QPalette pal = QGuiApplication::palette();
	QColor fgColor = pal.color(QPalette::WindowText);   

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
	p.setPen(fgColor);
	p.setBrush(fgColor);
	p.drawPolygon(a);
	p.setBrush(Qt::NoBrush);
	p.setPen(Qt::NoPen);

	pt = pos;
#undef LINETO
#undef LIGHT
#undef DARK	
}
