#include "containmentinterface.h"
#include "kmenuapplet.h"
#include "kmenu.h"

#include <QPoint>
#include <QRect>
#include <QQuickWindow>
#include <KPluginFactory>
#include <KConfigPropertyMap>

K_PLUGIN_CLASS_WITH_JSON(KMenuApplet, "metadata.json")

KMenuApplet::KMenuApplet(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args)
	: Plasma::Applet(parentObject, data, args),
	  m_containmentInterface(new ContainmentInterface(this)),
	  m_menu(new KMenu(this)) // this is fine since the applet is only used for pulling configs
{
	connect(m_menu, &QMenu::aboutToShow, this, [this]() {
		m_menuActive = true;
		Q_EMIT menuActiveChanged();
	});

	connect(m_menu, &QMenu::aboutToHide, this, [this]() {
		m_menuActive = false;
		Q_EMIT menuActiveChanged();
	});

	// Reinitialize menu on config changes
	connect(configuration(), &KConfigPropertyMap::valueChanged, this, [this]() {
		if (m_menu)
			m_menu->reinitialize();
	});
}

KMenuApplet::~KMenuApplet()
{
	delete m_menu;
}

void
KMenuApplet::showMenu(QQuickItem *button, Plasma::Types::Location panelLocation)
{
	if (!m_menu) {
		qWarning("KMenuApplet: KMenu not initialized!");
		return;
	}

	if (QQuickWindow *plasmoidWindow = button->window()) {
		m_menu->createWinId();
		if (QWindow *menuWindow = m_menu->windowHandle()) {
            menuWindow->setTransientParent(plasmoidWindow);
        }
	}

	m_menu->popup(popupPosition(button, panelLocation));
}

void
KMenuApplet::hideMenu()
{
	if (m_menu) m_menu->close(); 
}

QPoint
KMenuApplet::popupPosition(QQuickItem *item, Plasma::Types::Location panelLocation)
{
	if (!m_menu || !item || !item->window()) {
		return QPoint();
	}

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
