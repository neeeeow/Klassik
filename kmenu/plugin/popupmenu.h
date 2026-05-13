#pragma once

#include <QObject>
#include <QMenu>

#include <KService>


class PopupMenu : public QMenu
{
	Q_OBJECT;
	
public:
	explicit PopupMenu(QWidget *parent = nullptr);
	explicit PopupMenu(const QString &title, QWidget *parent = nullptr);
	~PopupMenu() override;

protected:	
	void cleanupActionList(QList<QAction *> &, QMenu * = nullptr);
	QAction* createActionFromService(KService::Ptr, QObject * = nullptr);
	QAction* createActionFromUrl(QUrl, QObject * = nullptr);
};
