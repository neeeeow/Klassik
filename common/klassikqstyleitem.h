/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "klassikpainteditem.h"

#include <QStyle>
#include <QStyleOption>
#include <QApplication>

class KlassikQStyleItem : public KlassikPaintedItem
{
	/* Template class for painting on to QML items with a QPainter. */

	Q_OBJECT
public:
	KlassikQStyleItem(QQuickItem *parent = nullptr) : KlassikPaintedItem(parent) {
		loadStyle();
	}
	virtual ~KlassikQStyleItem() = default;

protected:		
	QStyle* style() const { return m_style; }

private:
	QStyle *m_style;

	// Same as klassikpainteditem.h
	virtual void paint(QPainter *painter) const override = 0;

	// Functions for loading and handling changes in the active QStyle.
	void loadStyle() {
		m_style = qApp->style();
		if (m_style)
			connect(m_style, &QObject::destroyed, this, &KlassikQStyleItem::styleChanged, Qt::UniqueConnection);
	}	
	void styleChanged() {
		// we cannot simply connect to QEvent::StyleChange
		if (QCoreApplication::closingDown())
			return;
		loadStyle();
		if (m_style)
			updateImage();
	}
};
