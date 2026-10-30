/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "containmentinterface.h"
#include "servicemenuapplet.h"
#include "servicemenu.h"

#include <QQuickItem>
#include <QPoint>
#include <QRect>
#include <QQuickWindow>
#include <QTimer>

#include <KPluginFactory>
#include <KConfigPropertyMap>
#include <KConfigLoader>

#include <PlasmaQuick/AppletQuickItem>

ServiceMenuApplet::ServiceMenuApplet(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args)
	: Plasma::Applet(parentObject, data, args) {}

ServiceMenuApplet::~ServiceMenuApplet()
{
	deleteMenu();
}

void
ServiceMenuApplet::init()
{
	// init() should only ever be called once. If these pointers are not null, then something
	// has gone wrong.
	Q_ASSERT(!m_menu);
	Q_ASSERT(!m_containmentInterface);
	Q_ASSERT(!m_timer);
	
	// Call the original init function
	Plasma::Applet::init();

	// Set up the QTimer
	m_timer = new QTimer(this);
	m_timer->setSingleShot(true);
	m_timer->setInterval(0);
	if (configuration())
		connect(configuration(), &KConfigPropertyMap::valueChanged, m_timer, qOverload<>(&QTimer::start));		
	
	// Create the objects
	m_containmentInterface = new ContainmentInterface(this);
	initMenu();	
}

void
ServiceMenuApplet::initMenu()
{
	if (m_menu)
		deleteMenu();

	// Create the menu object. If it fails to create, simply bail out.
	m_menu = createMenu();
	if (!m_menu)
		return;

	// Let the applet know the menu's visiblity has changed
	connect(m_menu, &QMenu::aboutToShow, this, &ServiceMenuApplet::menuActiveChanged, Qt::QueuedConnection);
	connect(m_menu, &QMenu::aboutToHide, this, &ServiceMenuApplet::menuActiveChanged, Qt::QueuedConnection);

	// Reconfigure the menu when the timer changes
	if (m_timer)
		connect(m_timer, &QTimer::timeout, m_menu, &ServiceMenu::reinitialize);

	Q_EMIT hasMenuChanged();
}

void
ServiceMenuApplet::deleteMenu()
{
	if (!m_menu)
		return;

	m_menu->disconnect();
	m_menu->close();
	m_menu->deleteLater();
	m_menu.clear();

	Q_EMIT hasMenuChanged();
	Q_EMIT menuActiveChanged();
}

bool
ServiceMenuApplet::menuActive() const
{
	return m_menu && m_menu->isVisible();
}

ContainmentInterface*
ServiceMenuApplet::containmentInterface() const
{
	return m_containmentInterface;
}

QVariant
ServiceMenuApplet::configValue(const QString &key) const
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
ServiceMenuApplet::toggleMenu()
{
	auto *item = PlasmaQuick::AppletQuickItem::itemForApplet(this);
	if (!m_menu || !item)
		return;

	if (m_menu->isVisible())
		m_menu->close();
	else {
		m_menu->createWinId();
		if (QWindow *menuWindow = m_menu->windowHandle())
			menuWindow->setTransientParent(item->window());
		m_menu->refreshContents();
		m_menu->popup(popupPosition());
	}
}

QPoint
ServiceMenuApplet::popupPosition()
{
	auto *item = PlasmaQuick::AppletQuickItem::itemForApplet(this);
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
