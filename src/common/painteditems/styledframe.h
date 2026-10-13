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
	Q_PROPERTY(int lineWidth MEMBER m_lineWidth NOTIFY propertyChanged)
public:
	explicit StyledFrame(QQuickItem *parent = nullptr);
	
private:
	int m_lineWidth = 1;
	
	void paint(QPainter *p) const override;
};
