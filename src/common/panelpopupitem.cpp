/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "panelpopupitem.h"

#include <QWidget>
#include <QMenu>
#include <QWidgetAction>
#include <QQuickWindow>

PanelPopupItem::PanelPopupItem(QQuickItem *parent)
	: QQuickItem(parent), m_menu(new QMenu())
{
	// Set up the menu
	auto *action = new QWidgetAction(m_menu);
	auto *container = new QWidget(m_menu);
	container->setAttribute(Qt::WA_NoMousePropagation);
	m_layout = new QVBoxLayout(container);
	m_layout->setSpacing(0);
	m_layout->setContentsMargins(0,0,0,0);
	action->setDefaultWidget(container);
	m_menu->addAction(action);

	// Connect the aboutToShow signal to our function
	connect(m_menu, &QMenu::aboutToShow, this, &PanelPopupItem::aboutToShow);

	// Emit the activeChanged signal if we hide/show the menu, to allow
	// QML's state to update
	connect(m_menu, &QMenu::aboutToShow, this, &PanelPopupItem::activeChanged, Qt::QueuedConnection);
	connect(m_menu, &QMenu::aboutToHide, this, &PanelPopupItem::activeChanged, Qt::QueuedConnection);
}

PanelPopupItem::~PanelPopupItem()
{
	delete m_menu;
}

bool
PanelPopupItem::isActive() const
{
	return m_menu && m_menu->isVisible();
}

void
PanelPopupItem::componentComplete()
{
	QQuickItem::componentComplete();
	initContents(m_layout);
}

void
PanelPopupItem::togglePopup()
{
	if (!m_menu)
		return;

	if (m_menu->isVisible())
		m_menu->close();
	else {
		m_menu->createWinId();
		if (QWindow *popupWindow = m_menu->windowHandle())
			popupWindow->setTransientParent(window());
		m_menu->popup(popupPosition());
	}
}

QPoint
PanelPopupItem::popupPosition() const
{
	if (!m_menu || !window()) {
		return QPoint();
	}

	const QSize size = m_menu->sizeHint();
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
