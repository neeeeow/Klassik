/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "painteditem.h"

#include <QStyleOptionHeader>

class TaskBackground : public PaintedItem
{
	/*
	  This class draws a sunken KDE 3 style panel background
	*/
	  
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(bool sunken MEMBER m_sunken NOTIFY propertyChanged)
public:
	explicit TaskBackground(QQuickItem *parent = nullptr) : PaintedItem(parent) {}

private:
	bool m_sunken;

	void paint(QPainter *p) const override {		
		if (!style())
			return;

		QStyleOptionHeader header;
		header.palette = QGuiApplication::palette();
		if (m_sunken) {
			// Darken both the button & window color, since a QStyle might use either one
			// for drawing a CE_HeaderSection
			header.palette.setColor(QPalette::Button, header.palette.button().color().darker(110));
			header.palette.setColor(QPalette::Window, header.palette.window().color().darker(110));
		}
		header.rect = rect();
		header.state = QStyle::State_Enabled | (m_sunken ? QStyle::State_Sunken : QStyle::State_Raised);

		style()->drawControl(QStyle::CE_HeaderSection, &header, p);
	}
};
