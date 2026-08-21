#include "klassikstyle.h"

#include <QStyleOption>

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

		p->fillRect(x+2, y+2, w-4, h-4, opt->palette.button().color());
		
		p->restore();
		break;
	}

	// PUSH BUTTON
	// -------------------------------------------------------------------

	case PE_PanelButtonCommand: {
		
		break;
	}
		
	default: {
		QCommonStyle::drawPrimitive(pe, opt, p, widget);
		break;
	}
	}	
}
