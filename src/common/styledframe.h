/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "painteditem.h"

class StyledFrame : public PaintedItem
{
	/*
	   QML element which draws a frame using QStyle
	*/
	  
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)
	Q_PROPERTY(bool sunken READ sunken WRITE setSunken)
public:
	explicit StyledFrame(QQuickItem *parent = nullptr);

	int lineWidth() const { return m_lineWidth; }
	void setLineWidth(int lineWidth) {
		if (m_lineWidth != lineWidth) {
			m_lineWidth = lineWidth;
		    requestRepaint();
			Q_EMIT lineWidthChanged();
		}
	}

	bool sunken() const { return m_sunken; }
	void setSunken(bool sunken) {
		if (m_sunken != sunken) {
			m_sunken = sunken;
		    requestRepaint();
		}
	}

Q_SIGNALS:
	// We need this signal since lineWidth does get changed internally, and the QML side
	// must be aware of these changes to update item paddings
	void lineWidthChanged();
	
private:
	int m_lineWidth = 1;
	bool m_sunken = false;
	
	void paint(QPainter *p) const override;
};
