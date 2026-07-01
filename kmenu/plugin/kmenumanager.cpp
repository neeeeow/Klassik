#include "kmenumanager.h"
#include "kmenu.h"

#include <QPoint>
#include <QRect>
#include <QQuickWindow>

KMenuManager::KMenuManager(QObject *parent)
	: QObject(parent)
{
}

KMenuManager::~KMenuManager()
{
	if (m_menu)
		m_menu->deleteLater();
}

void
KMenuManager::initialize(QObject *containmentObject)
{
	m_containment = qobject_cast<Plasma::Containment*>(containmentObject);
	if (!m_containment)
		qWarning("KMenuManager: failed to obtain plasmoid containment!");
	
	if (!m_menu) {
		m_menu = new KMenu(m_containment);

	    connect(m_menu, &QMenu::aboutToShow, this, [this]() {
			m_menuActive = true;
			Q_EMIT menuActiveChanged();
		});

		connect(m_menu, &QMenu::aboutToHide, this, [this]() {
			m_menuActive = false;
			Q_EMIT menuActiveChanged();
		});
	}
}

void
KMenuManager::showMenu(QQuickItem *button, QQuickItem *root, panelLocation location)
{
	if (!m_menu) {
		qWarning("KMenuManager: KMenu not initialized!");
		return;
	}

	if (QQuickWindow *plasmoidWindow = button->window()) {
		m_menu->createWinId();
		if (QWindow *menuWindow = m_menu->windowHandle()) {
            menuWindow->setTransientParent(plasmoidWindow);
        }
	} 
	
	m_menu->popup(adjustedMenuPosition(button, root, location));
}

QPoint
KMenuManager::adjustedMenuPosition(QQuickItem *button, QQuickItem *root, panelLocation location)
{
	if (!m_menu)
		return QPoint(0,0); // should never be the case, but as a safeguard
	QSize menuSize = m_menu->sizeHint();

	// Get button coordinates relative to the screen
    QPoint btnGlobalPos = button->mapToGlobal(QPointF(0, 0)).toPoint(); // boundingRect()->topLeft() has x=0, y=0, so QPointF(0,0) is fine here
	int x = btnGlobalPos.x();
	int y = btnGlobalPos.y();

	// Get the panel coordinates relative to the screen
    QPoint rootGlobalPos = root->mapToGlobal(QPointF(0, 0)).toPoint();
	QRect rootRect = root->boundingRect().translated(rootGlobalPos).toRect();

	switch (location) {
	case TopEdge:
		y = rootRect.bottom();
		break;
	case BottomEdge:
		y = rootRect.top() - menuSize.height();
		break;
	case LeftEdge:
		x = rootRect.right();
		break;
	case RightEdge:
		x = rootRect.left() - menuSize.width();
		break;
	default:
		// Here, first try to place the menu above the panel, then below the panel, then to the right,
		// and finally to the left.
		if (!root->window())
			break;

		QScreen  *screen = root->window()->screen();
		if (!screen)
			break;
		
		QRect screenRect = screen->geometry();

		if ((rootRect.top() - screenRect.top()) >= menuSize.height()) // space above panel
			y = rootRect.top() - menuSize.height();
		else if ((screenRect.bottom()  - rootRect.bottom()) >= menuSize.height()) // space  below panel
			y = rootRect.bottom();
		else if ((screenRect.right() - rootRect.right()) >= menuSize.width()) // space to the right
			x = rootRect.right();
		else if ((rootRect.left() - screenRect.left()) >= menuSize.width()) // space to the bottom
			x = rootRect.left() - menuSize.width();
		
		break;
	}
	
	return QPoint(x,y);
}
