/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "panelpopupitem.h"

#include <QGridLayout>
#include <QApplication>
#include <QStyle>
#include <QQuickWindow>

#include <Plasma/Plasma>

PanelPopupItem::PanelPopupItem(QQuickItem *parent)
	: QQuickItem(parent), m_frame(new QFrame(nullptr, Qt::Popup))
{
	m_frame->setFrameStyle(QFrame::Raised | QFrame::StyledPanel);
	m_frame->installEventFilter(this);
	m_layout = new QVBoxLayout(m_frame);
	styleChanged();	
}

PanelPopupItem::~PanelPopupItem()
{
	if (m_frame) {
		m_frame->close();
		m_frame->deleteLater();		
	}
}

void
PanelPopupItem::componentComplete()
{
	QQuickItem::componentComplete();
	initContents(m_layout);
	m_initialized = true;
}

bool
PanelPopupItem::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == m_frame) {
		const auto type = event->type();
	    if (type == QEvent::Show || type == QEvent::Hide) {
			Q_EMIT activeChanged();
			if (m_initialized && type == QEvent::Show)
				aboutToShow();
		}
	}
	return QQuickItem::eventFilter(watched, event);
}

void
PanelPopupItem::styleChanged()
{
	if (QCoreApplication::closingDown())
		return;
	resetMargins();
	if (qApp->style())
		connect(qApp->style(), &QObject::destroyed, this, &PanelPopupItem::styleChanged, Qt::UniqueConnection);
}

void
PanelPopupItem::resetMargins()
{
	if (!m_layout || !m_frame)
		return;
	const int lineWidth = qApp->style() ? qApp->style()->pixelMetric(QStyle::PM_MenuPanelWidth) : 2;
	const int hMargin = qApp->style() ? qApp->style()->pixelMetric(QStyle::PM_MenuHMargin) : 0;
	const int vMargin = qApp->style() ? qApp->style()->pixelMetric(QStyle::PM_MenuVMargin) : 0;
	m_frame->setLineWidth(lineWidth);
	m_layout->setContentsMargins(hMargin, vMargin, hMargin, vMargin);
}

void
PanelPopupItem::togglePopup()
{
	if (!m_frame || !m_initialized)
		return;

	if (m_frame->isVisible())
		m_frame->close();
	else {
		m_frame->createWinId();
		if (QWindow *popupWindow = m_frame->windowHandle())
			popupWindow->setTransientParent(window());
		m_frame->move(popupPosition());
		m_frame->show();
	}
}

QPoint
PanelPopupItem::popupPosition() const
{
	if (!m_frame || !window()) {
		return QPoint();
	}

	const QSize size = m_frame->sizeHint();
	QPoint pos = mapToGlobal(QPointF(0, 0)).toPoint();
	QRect parentGeometryBounds(pos, QSize(width(), height()));

	const QPoint topPoint(pos.x(), parentGeometryBounds.top() - size.height());
    const QPoint bottomPoint(pos.x(), parentGeometryBounds.bottom());
    const QPoint leftPoint(parentGeometryBounds.left() - size.width(), pos.y());
    const QPoint rightPoint(parentGeometryBounds.right(), pos.y());

	QPoint dialogPos;
	switch (m_location) {
	case Plasma::Types::TopEdge:
	    dialogPos = bottomPoint;
		break;
	case Plasma::Types::LeftEdge:
	    dialogPos = rightPoint;
		break;
	case Plasma::Types::RightEdge:
	    dialogPos = leftPoint;
		break;
	default:
	    dialogPos = topPoint;
		break;
	}

	QRect avail = window()->screen()->availableGeometry();

    // If popup goes out of bounds...
    // ...at the left edge
    if (dialogPos.x() < avail.left()) {
        if (m_location != Plasma::Types::LeftEdge) {
            // move it in bounds
            // Note: floating popup goes here.
            dialogPos.setX(avail.left());
        } else {
            // flip it around
            dialogPos.setX(rightPoint.x());
        }
    }
    // ...at the right edge
    if (dialogPos.x() + size.width() > avail.right()) {
        if (m_location != Plasma::Types::RightEdge) {
            // move it in bounds
            // Note: floating popup goes here.
            dialogPos.setX(qMax(avail.left(), (avail.right() - size.width() + 1)));
        } else {
            // flip it around
            dialogPos.setX(leftPoint.x());
        }
    }
    // ...at the top edge
    if (dialogPos.y() < avail.top()) {
        if (m_location == Plasma::Types::LeftEdge || m_location == Plasma::Types::RightEdge) {
            // move it in bounds
            dialogPos.setY(avail.top());
        } else {
            // flip it around
            // Note: floating popup goes here.
            dialogPos.setY(bottomPoint.y());
        }
    }
    // ...at the bottom edge
    if (dialogPos.y() + size.height() > avail.bottom()) {
        if (m_location == Plasma::Types::LeftEdge || m_location == Plasma::Types::RightEdge) {
            // move it in bounds
            dialogPos.setY(qMax(avail.top(), (avail.bottom() - size.height() + 1)));
        } else {
            // flip it around
            // Note: floating popup goes here.
            dialogPos.setY(topPoint.y());
        }
    }
	
	return dialogPos;
}
