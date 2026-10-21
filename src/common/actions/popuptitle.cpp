/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "popuptitle.h"

#include <QPainter>
#include <QStyle>
#include <QStyleOptionHeader>

Widgets::PopupTitleWidget::PopupTitleWidget(const QString &title, const QIcon &icon, QWidget *parent)
	: QWidget(parent), m_title(title), m_icon(icon)
{
	setAttribute(Qt::WA_TransparentForMouseEvents);
}

void
Widgets::PopupTitleWidget::paintEvent(QPaintEvent *)
{
	QPainter p(this);

	// Draw the background
	QStyleOptionHeader header;
	header.initFrom(this);
	style()->drawControl(QStyle::CE_HeaderSection, &header, &p, this);

	// Set up QPainter
	QFont f(font());
	f.setBold(true);
	p.setFont(f);		

	// Rect for drawing the title text
	QRect titleRect;
	// Flags for drawing the text
	int tf;
	
	// Draw the contents
	if (!m_icon.isNull()) {
		const int iconExtent = style()->pixelMetric(QStyle::PM_SmallIconSize);
		const QRect iconRect = style()->visualRect(
			header.direction, rect(),
			QRect(4, 0, iconExtent, height()));
		titleRect = style()->visualRect(
			header.direction, rect(),
			QRect(iconExtent + 8, 0, width() - iconExtent - 12, height()));
		tf = Qt::AlignVCenter | Qt::TextSingleLine | ((header.direction == Qt::RightToLeft) ? Qt::AlignRight : Qt::AlignLeft);

		const QPixmap pixmap = m_icon.pixmap(QSize(iconExtent, iconExtent), devicePixelRatioF());
		QRect pmr(QPoint(0, 0), pixmap.size() / pixmap.devicePixelRatio());
		pmr.moveCenter(iconRect.center());		
		p.drawPixmap(pmr.topLeft(), pixmap);
	} else {
		titleRect = rect().adjusted(4,0,-4,0);
		tf = Qt::AlignCenter | Qt::TextSingleLine;
	}

	if (!m_title.isEmpty()) {
		p.setPen(palette().buttonText().color());
		p.drawText(titleRect, tf, m_title);
	}
}


QSize
Widgets::PopupTitleWidget::sizeHint() const
{
	QFont f(font());
	f.setBold(true);
	const QFontMetrics fm(f);
	const int iconExtent = style()->pixelMetric(QStyle::PM_SmallIconSize);
	const bool hasIcon = !m_icon.isNull();
	
	const int w = fm.horizontalAdvance(m_title) + (hasIcon ? iconExtent + 12 : 8);
	const int h = qMax(hasIcon ? iconExtent : 0, fm.height()) + 8;
	return QSize(w,h);
}
