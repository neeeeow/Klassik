#include "klassikstyle.h"
#include "bitmaps.h"

#include <QStyleOption>
#include <QGuiApplication>

#include <qdrawutil.h>

static qreal
getDpr(const QPainter *p) {		
	return p->device() ? p->device()->devicePixelRatio() : 1.0;
}

// Scales a QRect for drawing pixel-perfect lines on HiDPI displays
static QRect
getScaledRect(const QRect &rect, const qreal dpr) {
	return QRect(qRound(rect.x() * dpr), qRound(rect.y() * dpr), rect.width() * dpr, rect.height() * dpr);
}

static void
kDrawBeButton(QPainter *p, const QRect &r, const QPalette &pal,
							bool sunken, const QBrush *fill)
{	
	p->save();
	
	int x,y,w,h;
	const qreal dpr = getDpr(p);
	const qreal inverseScale = qreal(1) / dpr;
	bool isScaled = false;
	if (!qFuzzyCompare(dpr, qreal(1))) {
		isScaled = true;
	    getScaledRect(r, dpr).getRect(&x, &y, &w, &h);
		p->scale(inverseScale, inverseScale);
		p->translate(0.5, 0.5);
	} else {
	    r.getRect(&x, &y, &w, &h);
	}

	int x2 = x+w-1;
    int y2 = y+h-1;
    p->setPen(pal.dark().color());
    p->drawLine(x+1, y, x2-1, y);
    p->drawLine(x, y+1, x, y2-1);
    p->drawLine(x+1, y2, x2-1, y2);
    p->drawLine(x2, y+1, x2, y2-1);


    if(!sunken){
        p->setPen(pal.light().color());
        p->drawLine(x+2, y+2, x2-1, y+2);
        p->drawLine(x+2, y+3, x2-2, y+3);
        p->drawLine(x+2, y+4, x+2, y2-1);
        p->drawLine(x+3, y+4, x+3, y2-2);
    }
    else{
        p->setPen(pal.mid().color());
        p->drawLine(x+2, y+2, x2-1, y+2);
        p->drawLine(x+2, y+3, x2-2, y+3);
        p->drawLine(x+2, y+4, x+2, y2-1);
        p->drawLine(x+3, y+4, x+3, y2-2);
    }


    p->setPen(sunken? pal.light().color() : pal.mid().color());
    p->drawLine(x2-1, y+2, x2-1, y2-1);
    p->drawLine(x+2, y2-1, x2-1, y2-1);

    p->setPen(pal.mid().color());
    p->drawLine(x+1, y+1, x2-1, y+1);
    p->drawLine(x+1, y+2, x+1, y2-1);
    p->drawLine(x2-2, y+3, x2-2, y2-2);

    if(fill) {
		if (isScaled)
			p->translate(-0.5, -0.5);
        p->fillRect(x+4, y+4, w-6, h-6, *fill);
	}
	p->restore();
}

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

KlassikStyle::KlassikStyle(StyleType type)
	: QCommonStyle(), m_styleType(type)
{
}

void
KlassikStyle::polish(QWidget *widget)
{
	if (widget->inherits("QAbstractButton"))
		widget->setAttribute(Qt::WA_Hover, true);
	
	QCommonStyle::polish(widget);
}

void
KlassikStyle::renderGradient(QPainter *p, const QRect &r, const QColor &color,
							 bool horizontal) const
{
	if (m_styleType == HighColor) {
		const QColor ca = color.lighter(110);
		const QColor cb = color.darker(110);

		QLinearGradient gradient(0, 0, horizontal ? 1 : 0, horizontal ? 0 : 1);
		gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
		gradient.setColorAt(0, ca);
		gradient.setColorAt(1, cb);

		p->fillRect(r, gradient);
	} else
		p->fillRect(r, color);
}

void
KlassikStyle::drawPrimitive(QStyle::PrimitiveElement pe, const QStyleOption *opt,
							QPainter *p, const QWidget *widget) const
{
	// Fractional scaling set up
	const qreal dpr = getDpr(p);
	const qreal inverseScale = qreal(1) / dpr;
	QRect r; // Drawing rectangle for elements which must not be scaled
	bool isScaled = false;
	if (!qFuzzyCompare(dpr, qreal(1))) {
		isScaled = true;
	    r = getScaledRect(opt->rect, dpr);
	} else {
		r = opt->rect;
	}

	bool down = opt->state & State_Sunken;
	bool on   = opt->state & State_On;
	
	switch (pe) {
	// BUTTONS
	// -------------------------------------------------------------------
	case PE_FrameDefaultButton: {
		p->save();
		if (isScaled) {
			p->scale(inverseScale, inverseScale);
			p->translate(0.5, 0.5);
		}
		int x1, y1, x2, y2;
		r.getCoords( &x1, &y1, &x2, &y2 );
				
		// Button default indicator
		p->setPen( opt->palette.shadow().color() );
		p->drawLine( x1+1, y1, x2-1, y1 );
		p->drawLine( x1, y1+1, x1, y2-1 );
		p->drawLine( x1+1, y2, x2-1, y2 );
		p->drawLine( x2, y1+1, x2, y2-1 );

		p->restore();
		break;
	}

	case PE_IndicatorButtonDropDown:
	case PE_PanelButtonTool: {
		p->save();
		if (isScaled) {
			p->scale(inverseScale, inverseScale);
			p->translate(0.5, 0.5);
		}

		bool sunken = on || down;
		int  x,y,w,h;
		r.getRect(&x, &y, &w, &h);
		int x2 = x+w-1;
		int y2 = y+h-1;

		// Outer frame (round style)
		p->setPen(opt->palette.shadow().color());
		p->drawLine(x+1,y,x2-1,y);
		p->drawLine(x,y+1,x,y2-1);
		p->drawLine(x+1,y2,x2-1,y2);
		p->drawLine(x2,y+1,x2,y2-1);

		// Bevel
		p->setPen(sunken ? opt->palette.mid().color() : opt->palette.light().color());
		p->drawLine(x+1, y+1, x2-1, y+1);
		p->drawLine(x+1, y+1, x+1, y2-1);
		p->setPen(sunken ? opt->palette.light().color() : opt->palette.mid().color());
		p->drawLine(x+2, y2-1, x2-1, y2-1);
		p->drawLine(x2-1, y+2, x2-1, y2-1);

		p->fillRect(x+2, y+2, w-4, h-4, opt->palette.button());
		
		p->restore();
		break;
	}

	// PUSH BUTTON
	// -------------------------------------------------------------------
	case PE_PanelButtonCommand: {
		bool sunken = on || down;
		bool flat = !(opt->state & (State_Raised | State_Sunken));

		int  x, y, w, h;
		r.getRect(&x, &y, &w, &h);
		int x2 = x+w-1;
		int y2 = y+h-1;

		if (sunken)
			kDrawBeButton(p, opt->rect, opt->palette, true, &opt->palette.mid());
		else if (opt->state & State_MouseOver && !flat) {
			QBrush brush(opt->palette.button().color().lighter(110));
			kDrawBeButton(p, opt->rect, opt->palette, false, &brush);
		} else if (flat) {
			if ( opt->state & State_MouseOver )
				p->fillRect(opt->rect, opt->palette.button().color().lighter(110));		   

			p->save();			
			if (isScaled) {
				p->scale(inverseScale, inverseScale);
				p->translate(0.5, 0.5);
			}

			p->setPen(opt->palette.button().color().lighter(75));
			p->drawLine(x, y, x2, y);
			p->drawLine(x, y, x, y2);
			p->drawLine(x, y2, x2, y2);
			p->drawLine(x2, y, x2, y2);
			
			p->restore();
		} else if (m_styleType == HighColor) {
			p->save();			
			if (isScaled) {
				p->scale(inverseScale, inverseScale);
				p->translate(0.5, 0.5);
			}
			
			p->setPen(opt->palette.shadow().color());
			p->drawLine(x+1, y, x2-1, y);
			p->drawLine(x+1, y2, x2-1, y2);
			p->drawLine(x, y+1, x, y2-1);
			p->drawLine(x2, y+1, x2, y2-1);

			p->setPen(opt->palette.light().color());
			p->drawLine(x+2, y+2, x2-1, y+2);
			p->drawLine(x+2, y+3, x2-2, y+3);
			p->drawLine(x+2, y+4, x+2, y2-1);
			p->drawLine(x+3, y+4, x+3, y2-2);

			p->setPen(opt->palette.mid().color());
			p->drawLine(x2-1, y+2, x2-1, y2-1);
			p->drawLine(x+2, y2-1, x2-1, y2-1);

			p->drawLine(x+1, y+1, x2-1, y+1);
			p->drawLine(x+1, y+2, x+1, y2-1);
			p->drawLine(x2-2, y+3, x2-2, y2-2);

			if (isScaled)
				p->translate(-0.5, -0.5);
			renderGradient(p, QRect(x+4, y+4, w-6, h-6),
						   opt->palette.button().color(), false);
			p->restore();

		} else
			kDrawBeButton(p, opt->rect, opt->palette, false, &opt->palette.button());
		
		break;
	}

	// BEVELS
	// -------------------------------------------------------------------
	case PE_PanelButtonBevel: {
		p->save();			
		if (isScaled) {
			p->scale(inverseScale, inverseScale);
			p->translate(0.5, 0.5);
		}
		
		int x,y,w,h;
		r.getRect(&x, &y, &w, &h);
		bool sunken = on || down;
		int x2 = x+w-1;
		int y2 = y+h-1;

		// Outer frame
		p->setPen(opt->palette.shadow().color());
		p->drawRect(r.adjusted(0,0,-1,-1));

		// Bevel
		p->setPen(sunken ? opt->palette.mid().color() : opt->palette.light().color());
		p->drawLine(x+1, y+1, x2-1, y+1);
		p->drawLine(x+1, y+1, x+1, y2-1);
		p->setPen(sunken ? opt->palette.light().color() : opt->palette.mid().color());
		p->drawLine(x+2, y2-1, x2-1, y2-1);
		p->drawLine(x2-1, y+2, x2-1, y2-1);

		if (w > 4 && h > 4) {
			if (isScaled)
				p->translate(-0.5,-0.5);
			
			if (sunken)
				p->fillRect(x+2, y+2, w-4, h-4, opt->palette.button().color());
			else
				renderGradient( p, QRect(x+2, y+2, w-4, h-4),
								opt->palette.button().color(), opt->state & State_Horizontal );
		}

		p->restore();
		break;
	}

		
	// FOCUS RECT
	// -------------------------------------------------------------------
	case PE_FrameFocusRect: {
		if (const QStyleOptionFocusRect *fropt = qstyleoption_cast<const QStyleOptionFocusRect *>(opt)) {
            p->save();
			if (isScaled) {
				p->scale(inverseScale, inverseScale);
				p->translate(0.5, 0.5);
			}
			
            p->setBackgroundMode(Qt::TransparentMode);
            QColor bg_col = fropt->backgroundColor;
            if (!bg_col.isValid())
                bg_col = p->background().color();
            // Create an "XOR" color.
            QColor patternCol((bg_col.red() ^ 0xff) & 0xff,
                              (bg_col.green() ^ 0xff) & 0xff,
                              (bg_col.blue() ^ 0xff) & 0xff);
            p->setBrush(QBrush(patternCol, Qt::Dense4Pattern));
            p->setBrushOrigin(r.topLeft());
            p->setPen(Qt::NoPen);
            p->drawRect(r.left(), r.top(), r.width(), 1);    // Top
            p->drawRect(r.left(), r.bottom(), r.width(), 1); // Bottom
            p->drawRect(r.left(), r.top(), 1, r.height());   // Left
            p->drawRect(r.right(), r.top(), 1, r.height());  // Right
            p->restore();
        }
		break;
	}

		
	// CHECKBOX (indicator)
	// -------------------------------------------------------------------
	case PE_IndicatorCheckBox: {
		bool enabled  = opt->state & State_Enabled;
		bool nochange = opt->state & State_NoChange;
			
	    QBitmap xBmp = QBitmap::fromData(QSize(7, 7), x_bits, QImage::Format_MonoLSB);
		xBmp.setMask(xBmp);

		int x,y,w,h;
		x=r.x(); y=r.y(); w=r.width(); h=r.height();
		int x2 = x+w-1;
		int y2 = y+h-1;

		p->save();
		if (isScaled) {
			p->scale(inverseScale, inverseScale);
			p->translate(0.5, 0.5);
		}

		p->setPen(opt->palette.mid().color());
		p->drawLine(x, y, x2, y);
		p->drawLine(x, y, x, y2);

		p->setPen(opt->palette.light().color());
		p->drawLine(x2, y+1, x2, y2);
		p->drawLine(x+1, y2, x2, y2);

		p->setPen(opt->palette.shadow().color());
		p->drawLine(x+1, y+1, x2-1, y+1);
		p->drawLine(x+1, y+1, x+1, y2-1);

		p->setPen(opt->palette.midlight().color());
		p->drawLine(x2-1, y+2, x2-1, y2-1);
		p->drawLine(x+2, y2-1, x2-1, y2-1);

		if (isScaled)
			p->translate(-0.5, -0.5);
		if ( enabled )
			p->fillRect(x+2, y+2, w-4, h-4, 
						down ? opt->palette.button().color(): opt->palette.base());
		else
			p->fillRect(x+2, y+2, w-4, h-4, opt->palette.window());
		if (isScaled)
			p->translate(0.5, 0.5);

		if (!(opt->state & State_Off)) {
			if (on) {
				p->setPen(nochange ? opt->palette.dark().color() : opt->palette.text().color());
				int offset = w/2 - 3; // x and y offsets are the same since it's a square
				p->drawPixmap(x+offset, y+offset, xBmp);
			}
			else {
				p->setPen(opt->palette.shadow().color());
				p->drawRect(x+2, y+2, w-4, h-4);
				p->setPen(nochange ? opt->palette.text().color() : opt->palette.dark().color());
				p->drawLine(x+3, (y+h)/2-2, x+w-4, (y+h)/2-2);
				p->drawLine(x+3, (y+h)/2, x+w-4, (y+h)/2);
				p->drawLine(x+3, (y+h)/2+2, x+w-4, (y+h)/2+2);
			}
		}

		p->restore();
		break;
	}
		
	// RADIOBUTTON (exclusive indicator)
	// -------------------------------------------------------------------
	case PE_IndicatorRadioButton: {
		QBitmap centerBmp = QBitmap::fromData(QSize(13, 13), radiooff_center_bits, QImage::Format_MonoLSB);
		centerBmp.setMask( centerBmp );
		
		QPixmap radio(13,13);
		radio.fill(Qt::transparent);
		QPainter radioPainter(&radio);
		colorBitmaps(&radioPainter, opt->palette, 0, 0, radio.width(), radio.height(), true, radiooff_light_bits, radiooff_gray_bits,
					 nullptr, radiooff_dgray_bits, nullptr, nullptr);

		// The center fill of the indicator (grayed out when disabled)
		if ( opt->state & State_Enabled )
			radioPainter.setPen( down ? opt->palette.button().color() : opt->palette.base().color() );
		else
			radioPainter.setPen( opt->palette.window().color() );
		radioPainter.drawPixmap( 0, 0, centerBmp );

		// Indicator "dot"
		if ( on ) {
			QColor color = opt->state & State_NoChange ?
				opt->palette.dark().color() : opt->palette.text().color();
				
			radioPainter.setPen(color);
			radioPainter.drawLine(5, 4, 7, 4);
			radioPainter.drawLine(4, 5, 4, 7);
			radioPainter.drawLine(5, 8, 7, 8);
			radioPainter.drawLine(8, 5, 8, 7);
			radioPainter.fillRect(5, 5, 3, 3, color);
		}
		radioPainter.end();

		// Apply the radio button mask
		QBitmap mask = QBitmap::fromData(QSize(13, 13), radiomask_bits, QImage::Format_MonoLSB);
		radio.setMask(mask);

		p->save();
		if (isScaled)
			p->scale(inverseScale, inverseScale);

		int offset = r.width() / 2 - radio.width() / 2;
		
		p->drawPixmap(r.x() + offset, r.y() + offset, radio);
		p->restore();
		
		break;
	}

	// DOCKWINDOW HANDLES
	// -------------------------------------------------------------------
	case PE_IndicatorDockWidgetResizeHandle: {
		int x,y,w,h;
		r.getRect(&x, &y, &w, &h);
		int x2 = x+w-1;
		int y2 = y+h-1;

		p->save();
		if (isScaled) {
			p->scale(inverseScale, inverseScale);
			p->translate(0.5, 0.5);
		}

		p->setPen(opt->palette.dark().color());
		p->drawRect(x, y, w, h);
		p->setPen(opt->palette.window().color());
		p->drawPoint(x, y);
		p->drawPoint(x2, y);
		p->drawPoint(x, y2);
		p->drawPoint(x2, y2);
		p->setPen(opt->palette.light().color());
		p->drawLine(x+1, y+1, x+1, y2-1);
		p->drawLine(x+1, y+1, x2-1, y+1);
		p->setPen(opt->palette.midlight().color());
		p->drawLine(x+2, y+2, x+2, y2-2);
		p->drawLine(x+2, y+2, x2-2, y+2);
		p->setPen(opt->palette.mid().color());
		p->drawLine(x2-1, y+1, x2-1, y2-1);
		p->drawLine(x+1, y2-1, x2-1, y2-1);

		if (isScaled)
			p->translate(-0.5,-0.5);
		p->fillRect(x+3, y+3, w-5, h-5, opt->palette.window());
		p->restore();
		break;
	}

	// GENERAL PANELS / FRAMES
	// -------------------------------------------------------------------
	case PE_Frame:
	case PE_FrameMenu:
	case PE_FrameWindow:
	case PE_PanelLineEdit: {
		const QStyleOptionFrame *fopt = qstyleoption_cast<const QStyleOptionFrame *>(opt);
		if (!fopt)
			break;
		
		bool sunken  = opt->state & State_Sunken;
		int lw = fopt->lineWidth;
		if (lw == 2) {
			int x,y,w,h;
			r.getRect(&x, &y, &w, &h);
			int x2 = x+w-1;
			int y2 = y+h-1;

			p->save();
			if (isScaled) {
				p->scale(inverseScale, inverseScale);
				p->translate(0.5, 0.5);
			}
			
			p->setPen(sunken ? opt->palette.light().color() : opt->palette.dark().color());
			p->drawLine(x, y2, x2, y2);
			p->drawLine(x2, y, x2, y2);
			p->setPen(sunken ? opt->palette.mid().color() : opt->palette.light().color());
			p->drawLine(x, y, x2, y);
			p->drawLine(x, y, x, y2);
			p->setPen(sunken ? opt->palette.midlight().color() : opt->palette.mid().color());
			p->drawLine(x+1, y2-1, x2-1, y2-1);
			p->drawLine(x2-1, y+1, x2-1, y2-1);
			p->setPen(sunken ? opt->palette.dark().color() : opt->palette.midlight().color());
			p->drawLine(x+1, y+1, x2-1, y+1);
			p->drawLine(x+1, y+1, x+1, y2-1);
			p->restore();
		} else
		    QCommonStyle::drawPrimitive(pe, opt, p, widget);

		break;
	}

	case PE_PanelMenu: {
		p->fillRect(opt->rect, opt->palette.window());
		break;
	}

	// MENU / TOOLBAR PANEL
    // -------------------------------------------------------------------

	case PE_PanelMenuBar:
	case PE_FrameDockWidget: {
		int x,y,w,h;
		r.getRect(&x, &y, &w, &h);
		int x2 = x+w-1;
		int y2 = y+h-1;

		p->save();
		if (isScaled) {
			p->scale(inverseScale, inverseScale);
			p->translate(0.5, 0.5);
		}

		p->setPen(opt->palette.light().color());
		p->drawLine(r.x(), r.y(), x2-1,  r.y());
		p->drawLine(r.x(), r.y(), r.x(), y2-1);
		p->setPen(opt->palette.dark().color());
		p->drawLine(r.x(), y2, x2, y2);
		p->drawLine(x2, r.y(), x2, y2);

		if (isScaled)
			p->translate(-0.5,-0.5);

		// ### Qt should specify Style_Horizontal where appropriate
		renderGradient( p, QRect(r.x()+1, r.y()+1, r.width()-2, r.height()-2),
						opt->palette.button().color(), 
						(r.width() < r.height()) && (pe != PE_PanelMenuBar) );
		p->restore();
		
		break;
	}

	// TOOLBAR SEPARATOR
	// -------------------------------------------------------------------
	case PE_IndicatorToolBarSeparator: {
		renderGradient( p, opt->rect, opt->palette.button().color(),
						!(opt->state & State_Horizontal));

		p->save();
		if (isScaled) {
			p->scale(inverseScale, inverseScale);
			p->translate(0.5, 0.5);
		}

		if ( !(opt->state & State_Horizontal) ) {
			p->setPen(opt->palette.mid().color());
			p->drawLine(4, r.height()/2, r.width()-5, r.height()/2);
			p->setPen(opt->palette.light().color());
			p->drawLine(4, r.height()/2+1, r.width()-5, r.height()/2+1);
		} else {
			p->setPen(opt->palette.mid().color());
			p->drawLine(r.width()/2, 4, r.width()/2, r.height()-5);
			p->setPen(opt->palette.light().color());
			p->drawLine(r.width()/2+1, 4, r.width()/2+1, r.height()-5);
		}
		p->restore();
		break;
	}

	// ARROWS
	// -------------------------------------------------------------------
	case PE_IndicatorArrowUp:
	case PE_IndicatorArrowDown:
	case PE_IndicatorArrowLeft:
	case PE_IndicatorArrowRight: {
		QPolygon a;

		if ( m_styleType != B3 ) {
			// HighColor & Default arrows
			switch(pe) {
			case PE_IndicatorArrowUp:
				a.setPoints(QCOORDARRLEN(u_arrow), u_arrow);
				break;

			case PE_IndicatorArrowDown:
				a.setPoints(QCOORDARRLEN(d_arrow), d_arrow);
				break;

			case PE_IndicatorArrowLeft:
				a.setPoints(QCOORDARRLEN(l_arrow), l_arrow);
				break;

			default:
				a.setPoints(QCOORDARRLEN(r_arrow), r_arrow);
				break;
			}
		} else {
			// B3 arrows
			switch(pe) {
			case PE_IndicatorArrowUp:
				a.setPoints(QCOORDARRLEN(B3::u_arrow), B3::u_arrow);
				break;

			case PE_IndicatorArrowDown:
				a.setPoints(QCOORDARRLEN(B3::d_arrow), B3::d_arrow);
				break;

			case PE_IndicatorArrowLeft:
				a.setPoints(QCOORDARRLEN(B3::l_arrow), B3::l_arrow);
				break;

			default:
				a.setPoints(QCOORDARRLEN(B3::r_arrow), B3::r_arrow);
				break;
			}
		}

		p->save();
		if (isScaled) {
			p->scale(inverseScale, inverseScale);
			p->translate(0.5, 0.5);
		}
		if ( opt->state & State_Sunken )
			p->translate( pixelMetric( PM_ButtonShiftHorizontal ),
						  pixelMetric( PM_ButtonShiftVertical ) );

		if ( opt->state & State_Enabled ) {
			a.translate( r.x() + r.width() / 2, r.y() + r.height() / 2 );
			p->setPen( opt->palette.buttonText().color() );
			p->drawPolygon( a );
		} else {
			a.translate( r.x() + r.width() / 2 + 1, r.y() + r.height() / 2 + 1 );
			p->setPen( opt->palette.light().color() );
			p->drawPolygon( a );
			a.translate( -1, -1 );
			p->setPen( opt->palette.mid().color() );
			p->drawPolygon( a );
		}
		p->restore();		
		break;
	}

	// TOOLBAR HANDLE
	// -------------------------------------------------------------------
	case PE_IndicatorToolBarHandle: {
		int x = r.x(); int y = r.y();
		int x2 = r.x() + r.width()-1;
		int y2 = r.y() + r.height()-1;

		p->save();
		if (isScaled)
			p->scale(inverseScale, inverseScale);

		if (opt->state & State_Horizontal) {

			renderGradient( p, r, opt->palette.button().color(), false);
			if (isScaled)
				p->translate(0.5, 0.5);
			p->setPen(opt->palette.light().color());
			p->drawLine(x+1, y+4, x+1, y2-4);
			p->drawLine(x+3, y+4, x+3, y2-4);
			p->drawLine(x+5, y+4, x+5, y2-4);

			p->setPen(opt->palette.mid().color());
			p->drawLine(x+2, y+4, x+2, y2-4);
			p->drawLine(x+4, y+4, x+4, y2-4);
			p->drawLine(x+6, y+4, x+6, y2-4);

		} else {
				
			renderGradient( p, r, opt->palette.button().color(), true);
			if (isScaled)
				p->translate(0.5, 0.5);
			p->setPen(opt->palette.light().color());
			p->drawLine(x+4, y+1, x2-4, y+1);
			p->drawLine(x+4, y+3, x2-4, y+3);
			p->drawLine(x+4, y+5, x2-4, y+5);

			p->setPen(opt->palette.mid().color());
			p->drawLine(x+4, y+2, x2-4, y+2);
			p->drawLine(x+4, y+4, x2-4, y+4);
			p->drawLine(x+4, y+6, x2-4, y+6);

		}
		p->restore();
		break;
	}
		
	default: {
		QCommonStyle::drawPrimitive(pe, opt, p, widget);
		break;
	}
	}	
}

void
KlassikStyle::drawControl(ControlElement control, const QStyleOption *opt,
						  QPainter *p, const QWidget *widget) const
{
	const qreal dpr = getDpr(p);
	const qreal inverseScale = qreal(1) / dpr;
	QRect r; // Drawing rectangle for elements which must not be scaled
	bool isScaled = false;
	if (!qFuzzyCompare(dpr, qreal(1))) {
		isScaled = true;
	    r = getScaledRect(opt->rect, dpr);
	} else {
		r = opt->rect;
	}

	bool down = opt->state & State_Sunken;
	bool on   = opt->state & State_On;

	switch (control) {

	// HEADER SECTION
	// -------------------------------------------------------------------
	case CE_HeaderSection: {
		const QStyleOptionHeader *hdr = qstyleoption_cast<const QStyleOptionHeader *>(opt);
		if (!hdr)
			break;
		bool horizontal = hdr->orientation == Qt::Horizontal;

		int x,y,w,h;
		r.getRect(&x, &y, &w, &h);
		bool sunken = on || down;
		int x2 = x+w-1;
		int y2 = y+h-1;

		p->save();
		if (isScaled) {
			p->scale(inverseScale, inverseScale);
			p->translate(0.5, 0.5);
		}

		// Bevel
		p->setPen(sunken ? opt->palette.mid().color() : opt->palette.light().color());
		p->drawLine(x, y, x2-1, y);
		p->drawLine(x, y, x, y2-1);
		p->setPen(sunken ? opt->palette.light().color() : opt->palette.mid().color());
		p->drawLine(x+1, y2-1, x2-1, y2-1);
		p->drawLine(x2-1, y+1, x2-1, y2-1);
		p->setPen(opt->palette.shadow().color());
		p->drawLine(x, y2, x2, y2);
		p->drawLine(x2, y, x2, y2);

		if (isScaled)
			p->translate(-0.5,-0.5);
		
		if (sunken)
			p->fillRect(x+1, y+1, w-3, h-3, opt->palette.button());
		else
			renderGradient( p, QRect(x+1, y+1, w-3, h-3),
							opt->palette.button().color(), !horizontal );

		p->restore();
		break;
	}
		
	// SCROLLBAR
	// -------------------------------------------------------------------
	case CE_ScrollBarSlider: {
		QStyle::State flags = opt->state;
		flags ^= State_Horizontal;

		// Draw a button bevel
		QStyleOptionButton btn;
		btn.palette = opt->palette;
		btn.rect = opt->rect;
	    btn.state = (flags | State_Enabled | State_Raised) & ~State_Sunken;
		proxy()->drawPrimitive(PE_PanelButtonBevel, &btn, p, nullptr);

		p->save();
		if (isScaled) {
			p->scale(inverseScale, inverseScale);
			p->translate(0.5, 0.5);
		}

		if ( m_styleType != B3 ) {
			// HighColor & Default scrollbar
			if (flags & State_Horizontal) {
				if (r.height() >= 15) {
					int x = r.x()+3;
					int y = r.y() + (r.height()-7)/2;
					int x2 = r.right()-3;
					p->setPen(opt->palette.light().color());
					p->drawLine(x, y, x2, y);
					p->drawLine(x, y+3, x2, y+3);
					p->drawLine(x, y+6, x2, y+6);

					p->setPen(opt->palette.mid().color());
					p->drawLine(x, y+1, x2, y+1);
					p->drawLine(x, y+4, x2, y+4);
					p->drawLine(x, y+7, x2, y+7);
				}
			} else {
				if (r.width() >= 15) {
					int y = r.y()+3;
					int x = r.x() + (r.width()-7)/2;
					int y2 = r.bottom()-3;
					p->setPen(opt->palette.light().color());
					p->drawLine(x, y, x, y2);
					p->drawLine(x+3, y, x+3, y2);
					p->drawLine(x+6, y, x+6, y2);

					p->setPen(opt->palette.mid().color());
					p->drawLine(x+1, y, x+1, y2);
					p->drawLine(x+4, y, x+4, y2);
					p->drawLine(x+7, y, x+7, y2);
				}
			}
		} else {
			// B3 scrollbar
			if (flags & State_Horizontal) {
				int buttons = 0;
					
				if (r.height() >= 36) buttons = 3;
				else if (r.height() >=24) buttons = 2;
				else if (r.height() >=16) buttons = 1;
					
				int x = r.x() + (r.width()-7) / 2;
				int y = r.y() + (r.height() - (buttons * 5) -
								 (buttons-1)) / 2;
				int x2 = x + 7;
					
				for ( int i=0; i<buttons; i++, y+=6 )
				{
					p->setPen( opt->palette.mid().color() );
					p->drawLine( x+1, y, x2-1, y );
					p->drawLine( x, y+1, x, y+3 );
					p->setPen( opt->palette.light().color() );
					p->drawLine( x+1, y+1, x2-1, y+1 );
					p->drawLine( x+1, y+1, x+1, y+3 );
					p->setPen( opt->palette.dark().color() );
					p->drawLine( x+1, y+4, x2-1, y+4 );
					p->drawLine( x2, y+1, x2, y+3 );
				}
			} else {
				int buttons = 0;
					
				if (r.width() >= 36) buttons = 3;
				else if (r.width() >=24) buttons = 2;
				else if (r.width() >=16) buttons = 1;
					
				int x = r.x() + (r.width() - (buttons * 5) -
								 (buttons-1)) / 2;
				int y = r.y() + (r.height()-7) / 2;
				int y2 = y + 7;
					
				for ( int i=0; i<buttons; i++, x+=6 )
				{
					p->setPen( opt->palette.mid().color() );
					p->drawLine( x+1, y, x+3, y );
					p->drawLine( x, y+1, x, y2-1 );
					p->setPen( opt->palette.light().color() );
					p->drawLine( x+1, y+1, x+3, y+1 );
					p->drawLine( x+1, y+1, x+1, y2-1 );
					p->setPen( opt->palette.dark().color() );
					p->drawLine( x+1, y2, x+3, y2 );
					p->drawLine( x+4, y+1, x+4, y2-1 );
				}
			}
		}

		p->restore();
		break;
	}

	case CE_ScrollBarAddPage:
	case CE_ScrollBarSubPage: {
		int x, y, w, h;
		r.getRect(&x, &y, &w, &h);
		int x2 = x+w-1;
		int y2 = y+h-1;
		
		p->save();
		if (isScaled) {
			p->scale(inverseScale, inverseScale);
			p->translate(0.5, 0.5);
		}

		if ( m_styleType != B3 ) {
			// HighColor & Default scrollbar
				
			p->setPen(opt->palette.shadow().color());
				
			if (opt->state & State_Horizontal) {
				p->drawLine(x, y, x2, y);
				p->drawLine(x, y2, x2, y2);
				if (isScaled)
					p->translate(-0.5,-0.5);
				renderGradient(p, QRect(x, y+1, w, h-2),
							   opt->palette.mid().color(), false);
			} else {
				p->drawLine(x, y, x, y2);
				p->drawLine(x2, y, x2, y2);
				if (isScaled)
					p->translate(-0.5,-0.5);
				renderGradient(p, QRect(x+1, y, w-2, h),
							   opt->palette.mid().color(), true);
			}	
		} else {
			// B3 scrollbar
				
			p->setPen( opt->palette.mid().color() );
				
			if (opt->state & State_Horizontal) {
				p->drawLine(x, y, x2, y);
				p->drawLine(x, y2, x2, y2);
				if (isScaled)
					p->translate(-0.5,-0.5);
				p->fillRect( QRect(x, y+1, w, h-2), 
							 opt->state & State_Sunken ? opt->palette.button().color() : opt->palette.midlight() );
			} else {
				p->drawLine(x, y, x, y2);
				p->drawLine(x2, y, x2, y2);
				if (isScaled)
					p->translate(-0.5,-0.5);
				p->fillRect( QRect(x+1, y, w-2, h), 
							 opt->state & State_Sunken ? opt->palette.button().color() : opt->palette.midlight() );
			}
		}
		p->restore();
		break;
	}

	case CE_ScrollBarAddLine:
	case CE_ScrollBarSubLine: {
		// Draw the button background
		QStyleOptionButton btn;
		btn.palette = opt->palette;
		btn.rect = opt->rect;
	    btn.state = (opt->state & State_Enabled) | ((opt->state & State_Sunken) ? State_Sunken : State_Raised);
		proxy()->drawPrimitive(PE_PanelButtonBevel, &btn, p, nullptr);

		QStyleOption arrow;
		arrow.palette = opt->palette;
		arrow.rect = opt->rect;
		arrow.state = opt->state;
		
		PrimitiveElement pe;
		if (control == CE_ScrollBarAddLine)
			pe = (opt->state & State_Horizontal) ? PE_IndicatorArrowRight : PE_IndicatorArrowDown;
		else
			pe = (opt->state & State_Horizontal) ? PE_IndicatorArrowLeft : PE_IndicatorArrowUp;

		proxy()->drawPrimitive(pe, &arrow, p, nullptr);
		
		break;
	}

	// SPLITTER HANDLE
	// -------------------------------------------------------------------
	case CE_Splitter: {
		QStyleOption handle;
		handle.palette = opt->palette;
		handle.rect = opt->rect;
		handle.state = opt->state;
		proxy()->drawPrimitive(PE_IndicatorDockWidgetResizeHandle, &handle, p, nullptr);		
		break;
	}

	// PUSHBUTTON LABEL
	// -------------------------------------------------------------------
	case CE_PushButtonLabel: {
		const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(opt);
		if (!button)
			break;

		bool active = button->state & (State_On | State_Sunken);

		QRect textRect = button->rect;
		int tf = Qt::AlignVCenter | Qt::TextShowMnemonic;
		if (proxy()->styleHint(SH_UnderlineShortcut, button, widget))
			tf |= Qt::TextHideMnemonic;

		
		if (button->features & QStyleOptionButton::HasMenu) {
			int indicatorSize = proxy()->pixelMetric(PM_MenuButtonIndicator, button, widget);
			if (button->direction == Qt::LeftToRight)
				textRect = textRect.adjusted(0, 0, -indicatorSize, 0);
			else
				textRect = textRect.adjusted(indicatorSize, 0, 0, 0);
		}

		if (!button->icon.isNull()) {
			//Center both icon and text
			QIcon::Mode mode = button->state & State_Enabled ? QIcon::Normal : QIcon::Disabled;
			if (mode == QIcon::Normal && button->state & State_HasFocus)
				mode = QIcon::Active;
			QIcon::State state = QIcon::Off;
			if (button->state & State_On)
				state = QIcon::On;

			QPixmap pixmap = button->icon.pixmap(button->iconSize, dpr, mode, state);
			int pixmapWidth = pixmap.width() / pixmap.devicePixelRatio();
			int pixmapHeight = pixmap.height() / pixmap.devicePixelRatio();
			int labelWidth = pixmapWidth;
			int labelHeight = pixmapHeight;
			int iconSpacing = 4;//### 4 is currently hardcoded in QPushButton::sizeHint()
			if (!button->text.isEmpty()) {
				int textWidth = button->fontMetrics.boundingRect(opt->rect, tf, button->text).width();
				labelWidth += (textWidth + iconSpacing);
			}

			QRect iconRect = QRect(textRect.x() + (textRect.width() - labelWidth) / 2,
								   textRect.y() + (textRect.height() - labelHeight) / 2,
								   pixmapWidth, pixmapHeight);

			iconRect = visualRect(button->direction, textRect, iconRect);

			if (button->direction == Qt::RightToLeft)
				textRect.setRight(iconRect.left() - iconSpacing / 2);
			else
				textRect.setLeft(iconRect.left() + iconRect.width() + iconSpacing / 2);

			// qt_format_text reverses again when  painter->layoutDirection is also RightToLeft
			if (p->layoutDirection() == button->direction)
				tf |= Qt::AlignLeft;
			else
				tf |= Qt::AlignRight;

			if (active)
				iconRect.translate(proxy()->pixelMetric(PM_ButtonShiftHorizontal, opt, widget),
								   proxy()->pixelMetric(PM_ButtonShiftVertical, opt, widget));
			p->drawPixmap(iconRect, pixmap);
		} else {
			tf |= Qt::AlignHCenter;
		}
		if (active)
			textRect.translate(proxy()->pixelMetric(PM_ButtonShiftHorizontal, opt, widget),
							   proxy()->pixelMetric(PM_ButtonShiftVertical, opt, widget));

		if (active || (button->features & QStyleOptionButton::DefaultButton)) {
			int i;

			// Text shadow
			if (button->state & State_Enabled)
				for (i=0; i<2; i++)
					proxy()->drawItemText(p, textRect.translated(i + 1, 1), tf, button->palette, (button->state & State_Enabled),
										  button->text, active ? QPalette::Dark : QPalette::Mid);

			// Normal text
			for (i=0; i<2; i++)
				proxy()->drawItemText(p, textRect.translated(i,0), tf, button->palette, (button->state & State_Enabled),
									  button->text, active ? QPalette::Light : QPalette::ButtonText);
		} else
			proxy()->drawItemText(p, textRect, tf, button->palette, (button->state & State_Enabled),
								  button->text, QPalette::ButtonText);
				
		break;
	}

	// TOOLBOX TAB
	// -------------------------------------------------------------------
	case CE_ToolBoxTabShape: {
		bool pressed = opt->state & State_Sunken;
		bool selected = opt->state & State_Selected;
		int x, y, x2, y2;
		r.getCoords( &x, &y, &x2, &y2 );

		p->save();
		if (isScaled) {
			p->scale(inverseScale, inverseScale);
			p->translate(0.5, 0.5);
		}

		p->setPen( pressed ? opt->palette.shadow().color() : opt->palette.light().color() );
		p->drawLine( x, y, x2-1, y );
		p->drawLine( x, y, x, y2-1 );

		p->setPen( pressed ? opt->palette.light().color() : opt->palette.shadow().color() );
		p->drawLine( x, y2, x2, y2 );
		p->drawLine( x2, y, x2, y2 );

		QColor fill = selected ? opt->palette.highlight().color() : opt->palette.button().color();
		if (isScaled)
			p->translate(-0.5,-0.5);

		if ( pressed )
			p->fillRect( QRect(x+1, y+1, r.width()-2, r.height()-2), fill );
		else
			renderGradient(p, QRect(x+1, y+1, r.width()-2, r.height()-2),
						   fill, false);
		p->restore();
		break;
	}

	// TABBAR TAB
	// -------------------------------------------------------------------
	case CE_TabBarTabShape: {
		const QStyleOptionTab *tb = qstyleoption_cast<const QStyleOptionTab *>(opt);
		if (!tb)
			break;
		QTabBar::Shape tbs = tb->shape;
		bool selected      = tb->state & State_Selected;
		int x = r.x(), y=r.y(), bottom=r.bottom(), right=r.right();

		p->save();
		if (isScaled) {
			p->scale(inverseScale, inverseScale);
			p->translate(0.5, 0.5);
		}

		switch (tbs) {
		case QTabBar::RoundedNorth: {
			if (!selected)
				p->translate(0,1);
			p->setPen(selected ? tb->palette.light().color() : tb->palette.shadow().color());
			p->drawLine(x, y+4, x, bottom);
			p->drawLine(x, y+4, x+4, y);
			p->drawLine(x+4, y, right-1, y);
			if (selected)
				p->setPen(tb->palette.shadow().color());
			p->drawLine(right, y+1, right, bottom);

			p->setPen(tb->palette.midlight().color());
			p->drawLine(x+1, y+4, x+1, bottom);
			p->drawLine(x+1, y+4, x+4, y+1);
			p->drawLine(x+5, y+1, right-2, y+1);

			if (selected) {
				p->setPen(tb->palette.mid().color());
				p->drawLine(right-1, y+1, right-1, bottom);
				p->fillRect(x+2, y+4, r.width()-3, r.height()-4, tb->palette.window());
			} else {
				p->setPen(tb->palette.mid().color());
				p->drawPoint(right-1, y+1);
				p->drawLine(x+4, y+2, right-1, y+2);
				p->drawLine(x+3, y+3, right-1, y+3);
				if (isScaled)
					p->translate(-0.5,-0.5);
				p->fillRect(x+2, y+4, r.width()-3, r.height()-6, tb->palette.mid());
				if (isScaled)
					p->translate(0.5,0.5);

				p->setPen(tb->palette.light().color());
				p->drawLine(x, bottom-1, right, bottom-1);
				p->translate(0,-1);
			}

			break;
		}

		case QTabBar::RoundedSouth: {
			if (!selected)
				p->translate(0,-1);
			p->setPen(selected ? tb->palette.light().color() : tb->palette.shadow().color());
			p->drawLine(x, bottom-4, x, y);
			if (selected)
				p->setPen(tb->palette.mid().color());
			p->drawLine(x, bottom-4, x+4, bottom);
			if (selected)
				p->setPen(tb->palette.shadow().color());
			p->drawLine(x+4, bottom, right-1, bottom);
			p->drawLine(right, bottom-1, right, y);

			p->setPen(tb->palette.midlight().color());
			p->drawLine(x+1, bottom-4, x+1, y);
			p->drawLine(x+1, bottom-4, x+4, bottom-1);
			p->drawLine(x+5, bottom-1, right-2, bottom-1);

			if (selected) {
				p->setPen(tb->palette.mid().color());
				p->drawLine(right-1, y, right-1, bottom-1);
				p->fillRect(x+2, y, r.width()-3, r.height()-4, tb->palette.window());
			} else {
				p->setPen(tb->palette.mid().color());
				p->drawPoint(right-1, bottom-1);
				p->drawLine(x+4, bottom-2, right-1, bottom-2);
				p->drawLine(x+3, bottom-3, right-1, bottom-3);
				if (isScaled)
					p->translate(-0.5,-0.5);
				p->fillRect(x+2, y+2, r.width()-3, r.height()-6, tb->palette.mid());
				if (isScaled)
					p->translate(0.5,0.5);
				p->translate(0,1);
				p->setPen(tb->palette.dark().color());
				p->drawLine(x, y, right, y);
			}
			break;
		}

		case QTabBar::TriangularNorth: {
			if (!selected)
				p->translate(0,1);
			p->setPen(selected ? tb->palette.light().color() : tb->palette.shadow().color());
			p->drawLine(x, bottom, x, y+6);
			p->drawLine(x, y+6, x+6, y);
			p->drawLine(x+6, y, right-6, y);
			if (selected)
				p->setPen(tb->palette.mid().color());
			p->drawLine(right-5, y+1, right-1, y+5);
			p->setPen(tb->palette.shadow().color());
			p->drawLine(right, y+6, right, bottom);

			p->setPen(tb->palette.midlight().color());
			p->drawLine(x+1, bottom, x+1, y+6);
			p->drawLine(x+1, y+6, x+6, y+1);
			p->drawLine(x+6, y+1, right-6, y+1);
			p->drawLine(right-5, y+2, right-2, y+5);
			p->setPen(tb->palette.mid().color());
			p->drawLine(right-1, y+6, right-1, bottom);

			QPolygon a(6);
			a.setPoint(0, x+2, bottom);
			a.setPoint(1, x+2, y+7);
			a.setPoint(2, x+7, y+2);
			a.setPoint(3, right-7, y+2);
			a.setPoint(4, right-2, y+7);
			a.setPoint(5, right-2, bottom);
			p->setPen  (selected ? tb->palette.window().color() : tb->palette.mid().color());
			p->setBrush(selected ? tb->palette.window() : tb->palette.mid());
			p->drawPolygon(a);
			p->setBrush(Qt::NoBrush);
			if (!selected) {
				p->translate(0,-1);
				p->setPen(tb->palette.light().color());
				p->drawLine(x, bottom, right, bottom);
			}
			break;
		}

		case QTabBar::TriangularSouth: {
			if (!selected)
				p->translate(0,-1);
			p->setPen(selected ? tb->palette.light().color() : tb->palette.shadow().color());
			p->drawLine(x, y, x, bottom-6);
			if (selected)
				p->setPen(tb->palette.mid().color());
			p->drawLine(x, bottom-6, x+6, bottom);
			if (selected)
				p->setPen(tb->palette.shadow().color());
			p->drawLine(x+6, bottom, right-6, bottom);
			p->drawLine(right-5, bottom-1, right-1, bottom-5);
			if (!selected)
				p->setPen(tb->palette.shadow().color());
			p->drawLine(right, bottom-6, right, y);

			p->setPen(tb->palette.midlight().color());
			p->drawLine(x+1, y, x+1, bottom-6);
			p->drawLine(x+1, bottom-6, x+6, bottom-1);
			p->drawLine(x+6, bottom-1, right-6, bottom-1);
			p->drawLine(right-5, bottom-2, right-2, bottom-5);
			p->setPen(tb->palette.mid().color());
			p->drawLine(right-1, bottom-6, right-1, y);

			QPolygon a(6);
			a.setPoint(0, x+2, y);
			a.setPoint(1, x+2, bottom-7);
			a.setPoint(2, x+7, bottom-2);
			a.setPoint(3, right-7, bottom-2);
			a.setPoint(4, right-2, bottom-7);
			a.setPoint(5, right-2, y);
			p->setPen  (selected ? tb->palette.window().color() : tb->palette.mid().color());
			p->setBrush(selected ? tb->palette.window() : tb->palette.mid());
			p->drawPolygon(a);
			p->setBrush(Qt::NoBrush);
			if (!selected) {
				p->translate(0,1);
				p->setPen(tb->palette.dark().color());
				p->drawLine(x, y, right, y);
			}
			break;
		}
			
		default: {
			// TODO: Add the vertical tabs
			break;
		}
		}

		p->restore();
		break;
	}

	// MENUBAR BACKGROUND
	// -------------------------------------------------------------------
	case CE_MenuBarEmptyArea: {
		renderGradient(p, opt->rect, opt->palette.button().color(), false);
		break;
	}
	
	// MENUBAR ITEM (sunken panel on mouse over)
	// -------------------------------------------------------------------
	case CE_MenuBarItem:
	{
		const QStyleOptionMenuItem *menuitem = qstyleoption_cast<const QStyleOptionMenuItem *>(opt);
		if (!menuitem)
			break;	   

		if ((menuitem->state & State_Enabled) && (menuitem->state & State_Sunken)) // TODO: draw the non-selected background
			qDrawShadePanel(p, menuitem->rect, menuitem->palette, true,
							1, &menuitem->palette.midlight());

		int tf = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
		if (!proxy()->styleHint(SH_UnderlineShortcut, menuitem, widget))
			tf |= Qt::TextHideMnemonic;

		proxy()->drawItemText(p, menuitem->rect, tf, menuitem->palette,
							  menuitem->state & State_Enabled, menuitem->text);

		break;
	}

	// MENU ITEM
	// -------------------------------------------------------------------
	case CE_MenuItem: {
		const QStyleOptionMenuItem *menuitem = qstyleoption_cast<const QStyleOptionMenuItem *>(opt);
		if (!menuitem)
			break;

		const int tab = menuitem->reservedShortcutWidth;
		const int checkcol = qMax<int>(menuitem->maxIconWidth, 20);
		const int dim = proxy()->pixelMetric(PM_MenuButtonIndicator, opt, widget);
		const int itemHMargin  = 3;
		const int itemFrame    = 1;
		const int arrowHMargin = 6;
		
		bool enabled = menuitem->state & State_Enabled;
		bool checked = menuitem->checkType != QStyleOptionMenuItem::NotCheckable
			? menuitem->checked : false;
		bool active = menuitem->state & State_Selected;
		bool reverse = QGuiApplication::isRightToLeft();

		// Separator
		if ( menuitem->menuItemType == QStyleOptionMenuItem::Separator ) {
			p->save();
			if (isScaled) {
				p->scale(inverseScale, inverseScale);
				p->translate(0.5, 0.5);
			}

			int x,y,w,h;
			r.getRect(&x,&y,&w,&h);
			p->setPen( menuitem->palette.dark().color() );
			p->drawLine( x, y, x+w, y );
			p->setPen( menuitem->palette.light().color() );
			p->drawLine( x, y+1, x+w, y+1 );
			
			p->restore();
			break;
		}

		// Menu background
		if (active)
			qDrawShadePanel(p, menuitem->rect, menuitem->palette, true,
							1, &menuitem->palette.midlight());
		else
			p->fillRect(menuitem->rect, menuitem->palette.button());

		// Compute rects
		int x,y,w,h;
		menuitem->rect.getRect(&x,&y,&w,&h);
		QRect cr(x, y, checkcol, h); // Check mark rect
		QRect sr(x + w - arrowHMargin - 2*itemFrame - dim, y + h / 2 - dim / 2, dim, dim); // Sub menu indicator
		QRect tr(sr.left() - tab - itemHMargin, y, tab, h); // tab/accelerator text
		QRect ir(cr.right() + itemHMargin, y, tr.left() - cr.right() - 2 * itemHMargin - x, h); // text
		if ( reverse ) {
			cr = visualRect( opt->direction, menuitem->rect, cr );
			sr = visualRect( opt->direction, menuitem->rect, sr );
			tr = visualRect( opt->direction, menuitem->rect, tr );
			ir = visualRect( opt->direction, menuitem->rect, tr );
		}

		// Do we have an icon?
		if (!menuitem->icon.isNull()) {
			// If we have an icon and the menu is checked, draw a sunken frame
			// around the icon
			if (checked && !active)
				qDrawShadePanel(p, cr, menuitem->palette, true, 1,
								&menuitem->palette.midlight());
			// Draw the icon
			QIcon::Mode mode = enabled ? QIcon::Normal : QIcon::Disabled;
			if (active && enabled)
				mode = QIcon::Active;
			const auto size = proxy()->pixelMetric(PM_SmallIconSize, opt, widget);
			const auto state = checked ? QIcon::On : QIcon::Off;
			QPixmap pixmap = menuitem->icon.pixmap(QSize(size,size), dpr, mode, state);
			QRect pmr(QPoint(0, 0), pixmap.size() / pixmap.devicePixelRatio());
			pmr.moveCenter(cr.center());
			p->setPen(menuitem->palette.text().color());
			p->drawPixmap(pmr.topLeft(), pixmap);
		} else if (checked) { // Are we checked (without an icon)?
			// We only have to draw the background if the menu item is inactive -
			// if it's active the "pressed" background is already drawn
			if (!active)
				qDrawShadePanel(p, cr, menuitem->palette, true, 1,
								&menuitem->palette.midlight());
			// Draw the check mark
			QStyleOption check;
			check.state = State_None;
			check.state |= active ? State_Enabled : State_On;
			check.rect = cr;
			check.palette = menuitem->palette;
			proxy()->drawPrimitive(PE_IndicatorMenuCheckMark, &check, p, widget);
		}

		// Draw the text
		QStringView s(menuitem->text);
		if (!s.isEmpty()) {
			// Set up text
			qsizetype t = s.indexOf(u'\t');
			int tf = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;
			if (!proxy()->styleHint(SH_UnderlineShortcut, menuitem, widget))
				tf |= Qt::TextHideMnemonic;

			// draw accelerator/tab-text
			if (t >= 0) {
				const QString textToDraw = s.mid(t + 1).toString();
				int alignFlag = tf | ( reverse ? Qt::AlignLeft : Qt::AlignRight );
				proxy()->drawItemText(p, tr, alignFlag, menuitem->palette, enabled,
									  textToDraw, QPalette::ButtonText);
			}

			// Draw main item text
			const QString textToDraw = s.left(t).toString();
			int alignFlag = tf | ( reverse ? Qt::AlignRight : Qt::AlignLeft );
			proxy()->drawItemText(p, ir, alignFlag, menuitem->palette, enabled,
								  textToDraw, QPalette::ButtonText);
		}

		// Draw submenu indicator (if appropriate)
		if (menuitem->menuItemType == QStyleOptionMenuItem::SubMenu) {
			QStyleOption arrow;
			arrow.state = menuitem->state;
			arrow.rect = sr;
			arrow.palette = menuitem->palette;
			proxy()->drawPrimitive((reverse ? PE_IndicatorArrowLeft : PE_IndicatorArrowRight), &arrow, p, widget);
		}
		
		break;
	}
	
	default: {
		QCommonStyle::drawControl(control, opt, p, widget);
		break;
	}
	}
}

QRect
KlassikStyle::subElementRect(SubElement element, const QStyleOption *opt,
							   const QWidget *widget) const
{
	// We want the focus rect for buttons to be adjusted from
	// the Qt6 defaults to be similar to Qt 2's defaults.
	// -------------------------------------------------------------------
	if (element == SE_PushButtonFocusRect) {
		const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt);
		if (!btn)
			return QCommonStyle::subElementRect(element, opt, widget);

		QRect r;
		int dbw1 = 0, dbw2 = 0;
		if (btn->features & QStyleOptionButton::AutoDefaultButton){
			dbw1 = proxy()->pixelMetric(PM_ButtonDefaultIndicator, btn, widget);
			dbw2 = dbw1 * 2;
		}

		int dfw1 = proxy()->pixelMetric(PM_DefaultFrameWidth, btn, widget) + 1,
			dfw2 = dfw1 * 2;

		r.setRect(btn->rect.x() + dfw1 + dbw1 + 1,
				  btn->rect.y() + dfw1 + dbw1 + 1,
				  btn->rect.width() - dfw2 - dbw2 - 1,
				  btn->rect.height()- dfw2 - dbw2 - 1);
		r = visualRect(opt->direction, opt->rect, r);
		return r;
	} else
		return QCommonStyle::subElementRect(element, opt, widget);
}

void
KlassikStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *opt,
								   QPainter *p, const QWidget *widget) const
{
	const qreal dpr = getDpr(p);
	const qreal inverseScale = qreal(1) / dpr;
	bool isScaled = qFuzzyCompare(dpr, qreal(1)) ? false : true;

	switch (control) {
	// COMBOBOX
	// -------------------------------------------------------------------
	case CC_ComboBox: {
		const QStyleOptionComboBox *combobox = qstyleoption_cast<const QStyleOptionComboBox *>(opt);
		if (!combobox)
			break;
		
		bool sunken = (combobox->state & (State_On | State_Sunken));

		// QRect's corresponding to combobox frame, arrow, and entry field
		QRect frame, arrow, field;
		frame = proxy()->subControlRect(CC_ComboBox, combobox, SC_ComboBoxFrame, widget);
		arrow = proxy()->subControlRect(CC_ComboBox, combobox, SC_ComboBoxArrow, widget);
		field = proxy()->subControlRect(CC_ComboBox, combobox, SC_ComboBoxEditField, widget);

		// Draw combobox frame
		if ((opt->subControls & SC_ComboBoxFrame) && frame.isValid()) {
			int x,y,w,h;
			getScaledRect(frame, dpr).getRect(&x,&y,&w,&h);
			int x2 = x+w-1;
			int y2 = y+h-1;

			p->save();
			if (isScaled) {
				p->scale(inverseScale, inverseScale);
				p->translate(0.5, 0.5);
			}

			p->setPen(combobox->palette.shadow().color());
			p->drawLine(x+1, y, x2-1, y);
			p->drawLine(x+1, y2, x2-1, y2);
			p->drawLine(x, y+1, x, y2-1);
			p->drawLine(x2, y+1, x2, y2-1);

			// Ensure the edge notches are properly colored
			p->setPen(combobox->palette.button().color());
			p->drawPoint(x,y);
			p->drawPoint(x,y2);
			p->drawPoint(x2,y);
			p->drawPoint(x2,y2);

			if (isScaled)
				p->translate(-0.5,-0.5);			
			renderGradient( p, QRect(x+2, y+2, w-4, h-4),
							combobox->palette.button().color(), false);
			if (isScaled)
				p->translate(0.5,0.5);

			p->setPen(sunken ? combobox->palette.light().color() : combobox->palette.mid().color());
			p->drawLine(x2-1, y+2, x2-1, y2-1);
			p->drawLine(x+1, y2-1, x2-1, y2-1);

			p->setPen(sunken ? combobox->palette.mid().color() : combobox->palette.light().color());
			p->drawLine(x+1, y+1, x2-1, y+1);
			p->drawLine(x+1, y+2, x+1, y2-2);
			
			p->restore();
		}

		// Draw combobox arrow
	    if ((opt->subControls & SC_ComboBoxArrow) && arrow.isValid()) {
			QStyleOption arrowOpt;
			arrowOpt.state = combobox->state;
			arrowOpt.rect = arrow;
			arrowOpt.palette = combobox->palette;
			proxy()->drawPrimitive(PE_IndicatorArrowDown, &arrowOpt, p, widget);
		}

		// Draw combobox line edit
		if ((opt->subControls & SC_ComboBoxEditField) && field.isValid()) {
			if (combobox->editable)
				qDrawShadeRect(p, field.adjusted(-1,-1,1,1), combobox->palette, true);
			else if (combobox->state & State_HasFocus) {
				p->fillRect(field, combobox->palette.brush(QPalette::Highlight));
				QStyleOptionFocusRect focus;
				focus.QStyleOption::operator=(*combobox);
				focus.rect = subElementRect(SE_ComboBoxFocusRect, combobox, widget);
				focus.state |= State_FocusAtBorder;
				focus.backgroundColor = combobox->palette.highlight().color();
				proxy()->drawPrimitive(PE_FrameFocusRect, &focus, p, widget);
			}
		}
		
		break;
	}

	// SLIDER
	// -------------------------------------------------------------------
	case CC_Slider: {
		const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt);
		if (!slider)
			break;

		bool horizontal = slider->orientation == Qt::Horizontal;

		// Draw groove
		if (slider->subControls & SC_SliderGroove) {
			QRect r = getScaledRect(proxy()->subControlRect(CC_Slider, slider, SC_SliderGroove, widget), dpr);
			int gcenter = (horizontal ? r.height() : r.width()) / 2;

			QRect gr;
			if (horizontal)
				gr = QRect(r.x(), r.y()+gcenter-3, r.width(), 7);
			else
				gr = QRect(r.x()+gcenter-3, r.y(), 7, r.height());

			int x,y,w,h;
			gr.getRect(&x, &y, &w, &h);
			int x2=x+w-1;
			int y2=y+h-1;

			p->save();
			if (isScaled) {
				p->scale(inverseScale, inverseScale);
				p->translate(0.5, 0.5);
			}

			p->setPen(slider->palette.dark().color());
			p->drawLine(x+2, y, x2-2, y);
			p->drawLine(x, y+2, x, y2-2);
			if (isScaled)
				p->translate(-0.5,-0.5);
			p->fillRect(x+2,y+2,w-4, h-4, 
						(slider->state & State_Enabled) ? slider->palette.dark() : slider->palette.mid());
			if (isScaled)
				p->translate(0.5, 0.5);
			p->setPen(slider->palette.shadow().color());
			p->drawRect(x+1, y+1, w-3, h-3);
			p->setPen(slider->palette.light().color());
			p->drawPoint(x+1,y2-1);
			p->drawPoint(x2-1,y2-1);
			p->drawLine(x2, y+2, x2, y2-2);
			p->drawLine(x+2, y2, x2-2, y2);

			p->restore();
		}

		// Draw the handle
		if (slider->subControls & SC_SliderHandle) {
			QRect r = getScaledRect(proxy()->subControlRect(CC_Slider, slider, SC_SliderHandle, widget), dpr);
			int x,y,w,h;
			r.getRect(&x, &y, &w, &h);
			int x2 = x+w-1;
			int y2 = y+h-1;

			p->save();
			if (isScaled) {
				p->scale(inverseScale, inverseScale);
				p->translate(0.5, 0.5);
			}

			p->setPen(slider->palette.mid().color());
			p->drawLine(x+1, y, x2-1, y);
			p->drawLine(x, y+1, x, y2-1);
			p->setPen(slider->palette.shadow().color());
			p->drawLine(x+1, y2, x2-1, y2);
			p->drawLine(x2, y+1, x2, y2-1);

			p->setPen(slider->palette.light().color());
			p->drawLine(x+1, y+1, x2-1, y+1);
			p->drawLine(x+1, y+1, x+1,  y2-1);
			p->setPen(slider->palette.dark().color());
			p->drawLine(x+2, y2-1, x2-1, y2-1);
			p->drawLine(x2-1, y+2, x2-1, y2-1);
			p->setPen(slider->palette.midlight().color());
			p->drawLine(x+2, y+2, x2-2, y+2);
			p->drawLine(x+2, y+2, x+2, y2-2);
			p->setPen(slider->palette.mid().color());
			p->drawLine(x+3, y2-2, x2-2, y2-2);
			p->drawLine(x2-2, y+3, x2-2, y2-2);
			if (isScaled)
				p->translate(-0.5,-0.5);
			renderGradient(p, QRect(x+3, y+3, w-6, h-6), 
						   slider->palette.button().color(), !horizontal);
			if (isScaled)
				p->translate(0.5, 0.5);

			// Paint riffles
			if (horizontal) {
				p->setPen(slider->palette.light().color());
				p->drawLine(x+5, y+4, x+5, y2-4);
				p->drawLine(x+8, y+4, x+8, y2-4);
				p->drawLine(x+11,y+4, x+11, y2-4);
				p->setPen((slider->state & State_Enabled) ? slider->palette.shadow().color(): slider->palette.mid().color());
				p->drawLine(x+6, y+4, x+6, y2-4);
				p->drawLine(x+9, y+4, x+9, y2-4);
				p->drawLine(x+12,y+4, x+12, y2-4);
			} else {
				p->setPen(slider->palette.light().color());
				p->drawLine(x+4, y+5, x2-4, y+5);
				p->drawLine(x+4, y+8, x2-4, y+8);
				p->drawLine(x+4, y+11, x2-4, y+11);
			    p->setPen((slider->state & State_Enabled) ? slider->palette.shadow().color(): slider->palette.mid().color());
				p->drawLine(x+4, y+6, x2-4, y+6);
				p->drawLine(x+4, y+9, x2-4, y+9);
				p->drawLine(x+4, y+12, x2-4, y+12);
			}
			
			p->restore();
		}

		// Tick marks
		if (opt->subControls & SC_SliderTickmarks) {
			QStyleOptionSlider copy(*slider);
			copy.subControls = SC_SliderTickmarks;
			QCommonStyle::drawComplexControl(CC_Slider, &copy, p, widget);
		}
			
		break;
	}
		
	default: {
		QCommonStyle::drawComplexControl(control, opt, p, widget);
		break;
	}
	}
   
}

int
KlassikStyle::pixelMetric(PixelMetric metric, const QStyleOption *opt,
							const QWidget *widget) const
{
	int ret = 0;

	switch (metric) {
	// BUTTONS
	// -------------------------------------------------------------------
	case PM_ButtonMargin:
		ret = 4;
		break;

	case PM_ButtonDefaultIndicator: {
		if ( m_styleType == HighColor )
			ret = 0;
		else
			ret = 3;
		break;
	}

	case PM_MenuButtonIndicator: {
		if ( m_styleType != B3 )
			ret = 8;
		else
			ret = 7;
		break;
	}

	// CHECKBOXES / RADIO BUTTONS
	// -------------------------------------------------------------------
	case PM_ExclusiveIndicatorWidth:	// Radiobutton size
	case PM_ExclusiveIndicatorHeight:
	case PM_IndicatorWidth:				// Checkbox size
	case PM_IndicatorHeight: {
		ret = 13;						// 13x13
		break;
	}

	// TABS
	// ------------------------------------------------------------------------
	case PM_TabBarTabVSpace: {
		const QStyleOptionTab *tb = qstyleoption_cast<const QStyleOptionTab *>(opt);
		if (!tb)
			break;

		if ( tb->shape == QTabBar::RoundedNorth ||
			 tb->shape == QTabBar::RoundedSouth ||
			 tb->shape == QTabBar::TriangularNorth ||
			 tb->shape == QTabBar::TriangularSouth)
			ret = 10;
		else
			ret = 4;
		break;
	}

	case PM_TabBarTabOverlap: {
		const QStyleOptionTab *tb = qstyleoption_cast<const QStyleOptionTab *>(opt);
		if (!tb)
			break;

		if ( tb->shape == QTabBar::RoundedNorth ||
			 tb->shape == QTabBar::RoundedSouth ||
			 tb->shape == QTabBar::TriangularNorth ||
			 tb->shape == QTabBar::TriangularSouth)
			ret = 0;
		else
			ret = 2;
		break;
	}
		
	// SLIDER
	// ------------------------------------------------------------------------
	case PM_SliderLength:
		ret = 18;
		break;

	case PM_SliderThickness:
		ret = 24;
		break;

	case PM_SliderControlThickness: {
		const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt);
		if (!slider)
			break;

		int thickness = (slider->orientation == Qt::Horizontal) ?
			slider->rect.height() : slider->rect.width();

		if (slider->subControls & SC_SliderTickmarks)
			thickness = ((thickness*2)/3) + 3;

		ret = thickness;		
		break;
	}

	// SPLITTER
	// ------------------------------------------------------------------------
	case PM_SplitterWidth:
		ret = 6;
		break;

	// FRAMES
	// ------------------------------------------------------------------------
	case PM_MenuBarPanelWidth:
	case PM_DockWidgetFrameWidth:
		ret = 1;
		break;

	// GENERAL
	// ------------------------------------------------------------------------
	case PM_MaximumDragDistance:
		ret = -1;
		break;

	case PM_MenuBarItemSpacing:
		ret = 5;
		break;

	case PM_ToolBarItemSpacing:
		ret = 0;
		break;

	case PM_MenuScrollerHeight:
		ret = proxy()->pixelMetric( PM_ScrollBarExtent, opt, widget);
		break;
		
	default:
		ret = QCommonStyle::pixelMetric(metric, opt, widget);
		break;
	}
	
	return ret;
}
