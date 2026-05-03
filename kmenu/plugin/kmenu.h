#pragma once

#include <QObject>
#include <QMenu>
#include <QQuickItem>
#include <QtQml/qqml.h>

class KMenu : public QObject
{
	Q_OBJECT;
	
public:
	explicit KMenu(QObject *parent = nullptr);
	~KMenu() override;

	Q_INVOKABLE void showMenu();

private:
	QMenu *m_menu;

	void initialize();
};
