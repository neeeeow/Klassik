#include "kmenu.h"

#include <QPoint>

KMenu::KMenu(QObject *parent)
	: QObject(parent), m_menu(new QMenu())
{
	initialize(); // Populate menu items
}


KMenu::~KMenu() = default;

void
KMenu::initialize()
{
	// Just add some random items for testing
	QAction *test = m_menu->addAction("hello");
	m_menu->addSeparator();
	QAction *test1 = m_menu->addAction("world");
}

void
KMenu::showMenu()
{
	m_menu->popup(QPoint(0,0));
}
