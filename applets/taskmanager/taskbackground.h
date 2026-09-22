/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "../../common/klassikqstyleitem.h"

#include <QStyleOptionHeader>

class TaskBackground : public KlassikQStyleItem
{
	/*
	  This class draws a sunken KDE 3 style panel background
	*/
	  
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(bool sunken READ sunken WRITE setSunken)
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

private:
	bool m_sunken;

	void paint(QPainter *p) const override {		
		if (!style())
			return;

		QStyleOptionHeader header;
		header.palette = QGuiApplication::palette();
		if (m_sunken)
			header.palette.setColor(QPalette::Button, header.palette.button().color().darker(110));
	    header.rect = QRect(0,0,width(),height());
		header.state = m_sunken ? QStyle::State_Sunken : QStyle::State_Raised;

	    style()->drawControl(QStyle::CE_HeaderSection, &header, p);
	}
};
