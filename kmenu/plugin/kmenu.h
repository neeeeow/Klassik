#pragma once

#include <QObject>
#include <QMenu>
#include <QRect>
#include <QQuickItem>
#include <QtQml/qqml.h>

class KMenu : public QObject
{
	Q_OBJECT;
	
public:
	explicit KMenu(QObject *parent = nullptr);
	~KMenu() override;

	Q_INVOKABLE void showMenu(QQuickItem *, QQuickItem *, int);

private:
	QMenu *m_menu;

	void initialize();
	QPoint adjustedMenuPosition(QQuickItem *, QQuickItem *, int);
};
