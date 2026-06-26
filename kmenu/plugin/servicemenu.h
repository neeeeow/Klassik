#pragma once

#include "containmentinterface.h"

#include <QObject>
#include <QMenu>
#include <QPointer>

#include <KService>
#include <KServiceGroup>

class ServiceMenu : public QMenu
{
	Q_OBJECT;
	
public:
	explicit ServiceMenu(Plasma::Containment *containment, QWidget *parent = nullptr);
	explicit ServiceMenu(const QString &title, Plasma::Containment *containment, QWidget *parent = nullptr);
	~ServiceMenu() override;
	void initialize();

	void cleanupActionList(QList<QAction *> &);
	QAction* createActionFromService(const KService::Ptr &, const QUrl &url = QUrl());
	QAction* createActionFromUrl(const QUrl &);
	QAction* createFileExplorerActionFromUrl(const QUrl &url, const QIcon &icon, const QString &title);

protected:
	// Return the initialized flag
	bool initialized();

	// Set the initialized flag
	void setInitialized(bool);
	
	ContainmentInterface* containmentInterface() const { return m_containment; }

	QList<QAction*> actionListFromServiceGroup(const KServiceGroup::Ptr &root);

	static void runMenuEditor(QString arg = QString());
	static void invokeKRunner(QString arg = QString());
	
	void mousePressEvent(QMouseEvent *ev) override;
	void mouseMoveEvent(QMouseEvent *ev) override;

private:
	bool m_initialized; // Initialization state
	
	ContainmentInterface *m_containment;
	QPointF m_startPos;

	void showContextMenu(const QPoint &pos);
};
