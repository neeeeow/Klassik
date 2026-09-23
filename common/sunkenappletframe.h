/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "painteditem.h"

class SunkenAppletFrame : public PaintedItem
{
	/*
	   This class draws a sunken frame which appears on the panel
	   around various applets and buttons
	*/
	  
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)
public:
	SunkenAppletFrame(QQuickItem *parent = nullptr);
	~SunkenAppletFrame() override = default;

	int lineWidth() const { return m_lineWidth; }
	void setLineWidth(int lineWidth) {
		if (m_lineWidth != lineWidth) {
			m_lineWidth = lineWidth;
			updateImage();
			Q_EMIT lineWidthChanged();
		}
	}

Q_SIGNALS:
	// We need this signal since lineWidth does get changed internally, and the QML side
	// must be aware of these changes to update item paddings
	void lineWidthChanged();
	
private:
	int m_lineWidth = 1;
	
	void paint(QPainter *p) const override;
};
