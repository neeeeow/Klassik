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

	void cleanupActionList(QList<QAction *> &);
	QAction* createActionFromService(KService::Ptr);
	QAction* createActionFromUrl(QUrl);

protected:
	void mousePressEvent(QMouseEvent *) override;
	void mouseMoveEvent(QMouseEvent *) override;

private:
	QPointF m_startPos;

};
