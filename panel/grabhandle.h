/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "../common/klassikqstyleitem.h"

class GrabHandle : public KlassikQStyleItem
{
	/*
	  This class draws a grab handle for panel applets
	*/
	  
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(bool horizontal READ horizontal WRITE setHorizontal)
public:
    GrabHandle(QQuickItem *parent = nullptr) : KlassikQStyleItem(parent) {}
    ~GrabHandle() override = default;

	bool horizontal() const { return m_horizontal; }
	void setHorizontal(bool state) {
		if (m_horizontal != state) {
			m_horizontal = state;
			updateImage();
		}
	}

private:
	bool m_horizontal = true;

	void paint(QPainter *p) const override {		
		if (!style())
			return;

		QStyleOption opt;
		opt.palette = QGuiApplication::palette();
		opt.rect = QRect(0,0,width(),height());
		opt.state = QStyle::State_Enabled;
		if (m_horizontal)
			opt.state |= QStyle::State_Horizontal;
		style()->drawPrimitive(QStyle::PE_IndicatorToolBarHandle, &opt, p);
	}
};
