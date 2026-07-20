/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "../../common/klassikqstyleitem.h"

#include <QPalette>

class TaskBackground : public KlassikQStyleItem
{
	/*
	  This class draws a sunken KDE 3 style panel background
	*/
	  
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(bool sunken READ sunken WRITE setSunken);
public:
	TaskBackground(QQuickItem *parent = nullptr) : KlassikQStyleItem(parent) {}
	~TaskBackground() override = default;

	bool sunken() const { return m_sunken; }
	void setSunken(bool state) {
		if (m_sunken != state) {
			m_sunken = state;
			updateImage();
		}
	}

protected:	
	void paint(QPainter *p) const override {		
		if (!m_style)
			return;

		QStyleOption opt;
		opt.palette = QGuiApplication::palette();
		opt.rect = QRect(0,0,width(),height());
		opt.state = m_sunken ? QStyle::State_Sunken : QStyle::State_Raised;

	    m_style->drawControl(QStyle::CE_HeaderSection, &opt, p);
	}

private:
	bool m_sunken;
};
