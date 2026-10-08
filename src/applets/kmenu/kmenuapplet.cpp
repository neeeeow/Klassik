/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "containmentinterface.h"
#include "kmenuapplet.h"
#include "kmenu.h"

#include <QQuickItem>
#include <QPoint>
#include <QRect>
#include <QQuickWindow>
#include <QTimer>

#include <KPluginFactory>
#include <KConfigPropertyMap>
#include <KConfigLoader>

K_PLUGIN_CLASS_WITH_JSON(KMenuApplet, "metadata.json")

KMenuApplet::KMenuApplet(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args)
	: Plasma::Applet(parentObject, data, args)	 
{
}

KMenuApplet::~KMenuApplet()
{
    if (m_menu) {
        m_menu->disconnect(this);
        m_menu->close();
        m_menu->deleteLater();
    }
}

void
KMenuApplet::init()
{
	// init() should only ever be called once. If these pointers are not null, then something
	// has gone wrong.
	Q_ASSERT(!m_menu);
    Q_ASSERT(!m_containmentInterface);
	
	// Call the original init function
	Plasma::Applet::init();   
	
	// Create the objects
	m_containmentInterface = new ContainmentInterface(this);
	m_menu = new KMenu(this);

	// Connect signals
	connect(m_menu, &QMenu::aboutToShow, this, &KMenuApplet::menuActiveChanged, Qt::QueuedConnection);
	connect(m_menu, &QMenu::aboutToHide, this, &KMenuApplet::menuActiveChanged, Qt::QueuedConnection);

	// Set up a QTimer to deal with multiple config changes simultaneously
	// Firing the timer immediately on the next loop is sufficient.
	auto timer = new QTimer(this);
	timer->setSingleShot(true);
	timer->setInterval(0);
	connect(timer, &QTimer::timeout, this, [this]() {
		if (m_menu)
			m_menu->reinitialize();
	});
	if (configuration())
		connect(configuration(), &KConfigPropertyMap::valueChanged, timer, qOverload<>(&QTimer::start));
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

QVariant
KMenuApplet::configValue(const QString &key) const
{
	KConfigLoader *configLoader = configScheme();
	if (!configLoader)
		return QVariant();	
	
	const QVariant value = configLoader->property(key);
	if (!value.isValid()) {
		qWarning() << "key " << key << " not found!";
		return QVariant();
	}
	return value;
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
		m_menu->refreshIfDirty();
		m_menu->popup(popupPosition(button));
	}
}

QPoint
KMenuApplet::popupPosition(QQuickItem *item)
{
	if (!m_menu || !item || !item->window() || !item->window()->screen())
		return QPoint();

	const Plasma::Types::Location panelLocation = location();

	m_menu->ensurePolished();
	const QSize menuSize = m_menu->sizeHint();
	QPoint pos = item->mapToGlobal(QPointF(0, 0)).toPoint();
	QRect parentGeometryBounds(pos, item->size().toSize());

	const QPoint topPoint(pos.x(), parentGeometryBounds.top() - menuSize.height());
    const QPoint bottomPoint(pos.x(), parentGeometryBounds.bottom());
    const QPoint leftPoint(parentGeometryBounds.left() - menuSize.width(), pos.y());
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
    if (dialogPos.x() + menuSize.width() > avail.right()) {
        if (panelLocation != Plasma::Types::RightEdge) {
            // move it in bounds
            // Note: floating popup goes here.
            dialogPos.setX(qMax(avail.left(), (avail.right() - menuSize.width() + 1)));
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
    if (dialogPos.y() + menuSize.height() > avail.bottom()) {
        if (panelLocation == Plasma::Types::LeftEdge || panelLocation == Plasma::Types::RightEdge) {
            // move it in bounds
            dialogPos.setY(qMax(avail.top(), (avail.bottom() - menuSize.height() + 1)));
        } else {
            // flip it around
            // Note: floating popup goes here.
            dialogPos.setY(topPoint.y());
        }
    }
	
	return dialogPos;
}

#include "kmenuapplet.moc"
