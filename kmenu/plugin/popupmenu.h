#pragma once

#include "containmentinterface.h"

#include <QObject>
#include <QMenu>
#include <QPointer>

#include <KService>

class PopupMenu : public QMenu
{
	Q_OBJECT;
	
public:
	explicit PopupMenu(Plasma::Containment *containment, QWidget *parent = nullptr);
	explicit PopupMenu(const QString &title, Plasma::Containment *containment, QWidget *parent = nullptr);
	~PopupMenu() override;

	void cleanupActionList(QList<QAction *> &);
	QAction* createActionFromService(const KService::Ptr &, const QUrl &url = QUrl(), QWidget *parent = nullptr);
	QAction* createActionFromUrl(const QUrl &, QWidget *parent = nullptr);

protected:
	ContainmentInterface* containmentInterface() const { return m_containment; }
	
	void mousePressEvent(QMouseEvent *) override;
	void mouseMoveEvent(QMouseEvent *) override;

private:
	ContainmentInterface *m_containment;
	
	QPointF m_startPos;

	void initialize();
	void showContextMenu(const QPoint &);
	void runMenuEditor(QString);
};
