#pragma once

#include "servicemenu.h"
#include "popupmenutitle.h"

#include <QObject>

#include <PlasmaActivities/Stats/ResultModel>

#include <sessionmanagement.h>

class KMenu : public ServiceMenu
{
	Q_OBJECT;
	
public:
	explicit KMenu(KMenuApplet *applet, QWidget *parent = nullptr);
	~KMenu() override = default;

	// Resets the menu
	void reinitialize() override;

protected:
	bool event(QEvent *e) override;
	void changeEvent(QEvent *event) override;
	void paintEvent(QPaintEvent *e) override;

	void mousePressEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent *e) override;
	void mouseMoveEvent(QMouseEvent *e) override;

private:	
	// Session manager
	SessionManagement m_session;
	
	// Side pixmaps
	QPixmap m_sidePixmap;
	QPixmap m_sideTilePixmap;
	
	// Action to which we anchor the applications 
	QAction *m_applicationsAnchor = nullptr;

	// ResultModels for recent apps
    KActivities::Stats::ResultModel *m_recentApps = nullptr;

	// Action lists
	QList<QAction *> m_recentActions; // List of actions linking to recent applications
	QList<QAction *> m_recentDocumentsActions; // List of actions linking to recent documents
	QList<QAction *> m_applicationActions; // List of actions in the root of the applications list

	// Initializes the menu (automatically done during construction)
	void initialize() override;

	// Sets the margin dependant on the side pixmap setting
	void setMargins();

	// Side pixmap related functions
	void loadSidePixmap();
	QRect sideImageRect();
	void colorize(QImage &image);

	// Functions for populating the menu
	void createRecentMenuItems();
	void updateRecent();
	void createApplicationsItems();
	void updateSearchResults();
	void updateApplications();

	QMouseEvent* translateMouseEvent( QMouseEvent* e );

	static inline QRect getScaledRect(const QRect &rect, const qreal dpr) {
		return QRect(qRound(rect.x() * dpr), qRound(rect.y() * dpr), rect.width() * dpr, rect.height() * dpr);
	}
};
