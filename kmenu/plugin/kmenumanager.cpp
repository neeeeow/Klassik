#include "kmenumanager.h"
#include "kmenu.h"

#include <QPoint>
#include <QRect>

KMenuManager::KMenuManager(QObject *parent)
	: QObject(parent), m_menu(new KMenu())
{
}

KMenuManager::~KMenuManager() = default;

void
KMenuManager::showMenu(QQuickItem *button, QQuickItem *root, int location)
{	
	m_menu->popup(adjustedMenuPosition(button, root, location));
}

QPoint
KMenuManager::adjustedMenuPosition(QQuickItem *button, QQuickItem *root, int location)
{	
	QSize menuSize = m_menu->sizeHint();

	// Get button coordinates relative to the screen
    QPoint btnGlobalPos = button->mapToGlobal(QPointF(0, 0)).toPoint(); // boundingRect()->topLeft() has x=0, y=0, so QPointF(0,0) is fine here
	int x = btnGlobalPos.x();
	int y = btnGlobalPos.y();

	// Get the panel coordinates relative to the screen
    QPoint rootGlobalPos = root->mapToGlobal(QPointF(0, 0)).toPoint();
	QRect rootRect = root->boundingRect().translated(rootGlobalPos).toRect();

	switch (location) {
	case 3: // Top edge
		y = rootRect.bottom();
		break;
	case 4: // Bottom edge
		y = rootRect.top() - menuSize.height();
		break;
	case 5: // Left edge
		x = rootRect.right();
		break;
	case 6: // Right edge
		x = rootRect.left() - menuSize.width();
		break;
	default:
		y -= menuSize.height();
		break;
	}
	
	return QPoint(x,y);
}
