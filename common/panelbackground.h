/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "klassikqstyleitem.h"

#include <QPalette>
#include <QStyleOptionFrame>
#include <QPainter>
#include <QImage>

class PanelBackground : public KlassikQStyleItem
{
	/*
	  This class draws a sunken KDE 3 style panel background
	*/
	  
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(bool useBackground READ useBackground WRITE setUseBackground)
public:
	PanelBackground(QQuickItem *parent = nullptr) : KlassikQStyleItem(parent) {}
	~PanelBackground() override = default;

	bool useBackground() const { return m_useBackground; }
	void setUseBackground(bool state) {
		if (m_useBackground != state) {
			m_useBackground = state;
			updateImage();
		}
	}

protected:	
	void paint(QPainter *p) const override {		
		QRect r = QRect(0,0,width(),height());		
		QPalette pal = QGuiApplication::palette();

		if (m_useBackground) {
			QImage image;
			image.load(QStringLiteral(":/qt/qml/plasma/applet/com/github/neeeeow/klassik/panel/defaultBackground.png"));
			if (!image.isNull())
				p->drawImage(r, image);
		} else {
			p->fillRect(r, pal.window());
		}

		if (!m_style)
			return;
		
		QStyleOptionFrame opt;
		opt.palette = pal;
		opt.rect = r;
		opt.state = QStyle::State_Enabled | QStyle::State_Raised;
		opt.features = QStyleOptionFrame::None;
		opt.frameShape = QFrame::StyledPanel;
		opt.lineWidth = 2;

		m_style->drawControl(QStyle::CE_ShapedFrame, &opt, p);
	}

private:

	bool m_useBackground = false;
};
