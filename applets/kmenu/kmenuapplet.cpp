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

KMenuApplet::MenuLocation
KMenuApplet::preferredMenuLocation(QQuickItem *root, Plasma::Types::Location panelLocation)
{
	if (!m_menu)
		return Above;   

	switch (panelLocation) {
	case Plasma::Types::TopEdge:
	    return Below;
	case Plasma::Types::BottomEdge:
	    return Above;
	case Plasma::Types::LeftEdge:
	    return Right;
	case Plasma::Types::RightEdge:
		return Left;
	default:
		// Here, first try to place the menu above the panel, then below the panel, then to the right,
		// and finally to the left.
		if (!root->window())
			break;

		QScreen  *screen = root->window()->screen();
		if (!screen)
			break;

		QSize menuSize = m_menu->sizeHint();

		// Get the panel coordinates relative to the screen
		QPoint rootGlobalPos = root->mapToGlobal(QPointF(0, 0)).toPoint();
		QRect rootRect = root->boundingRect().translated(rootGlobalPos).toRect();
		
		QRect screenRect = screen->geometry();

		if ((rootRect.top() - screenRect.top()) >= menuSize.height()) // space above panel
		    return Above;
		else if ((screenRect.bottom()  - rootRect.bottom()) >= menuSize.height()) // space  below panel
		    return Below;
		else if ((screenRect.right() - rootRect.right()) >= menuSize.width()) // space to the right
		    return Right;
		else if ((rootRect.left() - screenRect.left()) >= menuSize.width()) // space to the bottom
		    return Left;
		
		break;
	}

	return Above;
}

void
KMenuApplet::showMenu(QQuickItem *button, QQuickItem *root, Plasma::Types::Location panelLocation)
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
	
	m_menu->popup(adjustedMenuPosition(button, root, panelLocation));
}

void
KMenuApplet::hideMenu()
{
	if (m_menu) m_menu->close(); 
}

QPoint
KMenuApplet::adjustedMenuPosition(QQuickItem *button, QQuickItem *root, Plasma::Types::Location panelLocation)
{
	MenuLocation location = preferredMenuLocation(root, panelLocation);
	QSize menuSize = m_menu->sizeHint();
	
	// Get button coordinates relative to the screen
    QPoint btnGlobalPos = button->mapToGlobal(QPointF(0, 0)).toPoint(); // boundingRect()->topLeft() has x=0, y=0, so QPointF(0,0) is fine here
	int x = btnGlobalPos.x();
	int y = btnGlobalPos.y();

	// Get the panel coordinates relative to the screen
    QPoint rootGlobalPos = root->mapToGlobal(QPointF(0, 0)).toPoint();
	QRect rootRect = root->boundingRect().translated(rootGlobalPos).toRect();

	switch (location) {
	case Above:
		y = rootRect.top() - menuSize.height();
		break;
	case Below:
		y = rootRect.bottom();
		break;
	case Right:
		x = rootRect.right();
		break;
	case Left:
		x = rootRect.left() - menuSize.width();
		break;
	}

	return QPoint(x, y);
}

#include "kmenuapplet.moc"
