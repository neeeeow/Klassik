/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "containmentinterface.h"
#include "kmenuapplet.h"
#include "kmenu.h"

#include <QPoint>
#include <QRect>
#include <QQuickWindow>
#include <QTimer>

#include <KPluginFactory>
#include <KConfigPropertyMap>

K_PLUGIN_CLASS_WITH_JSON(KMenuApplet, "metadata.json")

KMenuApplet::KMenuApplet(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args)
	: Plasma::Applet(parentObject, data, args)	 
{
}

KMenuApplet::~KMenuApplet()
{
	if (m_menu) {
		m_menu->close();
		m_menu->deleteLater();
	}
}

void
KMenuApplet::init()
{
	// Call the original init function
	Plasma::Applet::init();
	
	// init() should only ever be called once, but we check to see if
	// m_menu and m_containmentInterface exist just in case.
	if (m_menu) {
		m_menu->close();
		m_menu->deleteLater();		
	}
	if (m_containmentInterface)
		m_containmentInterface->deleteLater();
	
	// Create the objects
	m_containmentInterface = new ContainmentInterface(this);
	m_menu = new KMenu(this);

	// Connect signals
	connect(m_menu, &QMenu::aboutToShow, this, &KMenuApplet::menuActiveChanged, Qt::QueuedConnection);
	connect(m_menu, &QMenu::aboutToHide, this, &KMenuApplet::menuActiveChanged, Qt::QueuedConnection);

	// Set up a QTimer to deal with multiple config changes simultaneously
	// Firing the timer immediately on the next loop is sufficient.
	if (m_reinitTimer)
		m_reinitTimer->deleteLater();
	m_reinitTimer = new QTimer(this);
	m_reinitTimer->setSingleShot(true);
	m_reinitTimer->setInterval(0);
	connect(m_reinitTimer, &QTimer::timeout, this, [this]() {
		if (m_menu)
			m_menu->reinitialize();
	});	
	connect(configuration(), &KConfigPropertyMap::valueChanged, this, [this]() {
		m_reinitTimer->start();
	});
}

bool
KMenuApplet::isMenuActive() const
{
	return m_menu && m_menu->isVisible();
}

ContainmentInterface*
KMenuApplet::containmentInterface() const
{
	return m_containmentInterface;
}

void
KMenuApplet::toggleMenu(QQuickItem *button)
{
	if (!m_menu || !button)
		return;

	if (m_menu->isVisible())
		m_menu->close();
	else {
		m_menu->createWinId();
		if (QWindow *menuWindow = m_menu->windowHandle())
			menuWindow->setTransientParent(button->window());
		m_menu->popup(popupPosition(button));
	}
}

QPoint
KMenuApplet::popupPosition(QQuickItem *item)
{
	if (!m_menu || !item || !item->window())
		return QPoint();

	const Plasma::Types::Location panelLocation = location();
	
	const QSize size = m_menu->sizeHint();
	QPoint pos = item->mapToGlobal(QPointF(0, 0)).toPoint();
	QRect parentGeometryBounds(pos, QSize(item->width(), item->height()));

	const QPoint topPoint(pos.x(), parentGeometryBounds.top() - size.height());
    const QPoint bottomPoint(pos.x(), parentGeometryBounds.bottom());
    const QPoint leftPoint(parentGeometryBounds.left() - size.width(), pos.y());
    const QPoint rightPoint(parentGeometryBounds.right(), pos.y());

	QPoint dialogPos;
	switch (panelLocation) {
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

	QRect avail = item->window()->screen()->availableGeometry();

    // If popup goes out of bounds...
    // ...at the left edge
    if (dialogPos.x() < avail.left()) {
        if (panelLocation != Plasma::Types::LeftEdge) {
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
        if (panelLocation != Plasma::Types::RightEdge) {
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
        if (panelLocation == Plasma::Types::LeftEdge || panelLocation == Plasma::Types::RightEdge) {
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
        if (panelLocation == Plasma::Types::LeftEdge || panelLocation == Plasma::Types::RightEdge) {
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

#include "kmenuapplet.moc"
