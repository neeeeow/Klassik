/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "kmenu.h"
#include "kmenuapplet.h"
#include "popupmenutitle.h"
#include "popupmenusearch.h"
#include "recentdocsmenu.h"
#include "settingsmenu.h"
#include "systemmenu.h"

#include <QPainter>
#include <QPoint>
#include <QEvent>
#include <QAction>
#include <QLineEdit>
#include <QRect>
#include <QToolButton>
#include <QPaintEvent>

#include <KColorScheme>
#include <KLocalizedString>
#include <KService>
#include <KServiceGroup>
#include <KSycoca>

#include <Plasma/Plasma>
#include <PlasmaActivities/Stats/Query>

KMenu::KMenu(KMenuApplet *applet, QWidget *parent)
	: ServiceMenu(applet, parent),
	  m_session(this)
{
	initialize(); // Populate menu items
}

void
KMenu::changeEvent(QEvent *e)
{
	if (e->type() == QEvent::PaletteChange && !m_sidePixmap.isNull() && !m_sideTilePixmap.isNull()) {
		loadSidePixmap();
		update();
	}
	ServiceMenu::changeEvent(e);
}

void
KMenu::paintEvent(QPaintEvent *e)
{
	ServiceMenu::paintEvent(e);
	if (m_sidePixmap.isNull() || m_sideTilePixmap.isNull() || !m_config.drawSideImage)
		return;

	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing, true);

	QRect r = sideImageRect();
	r.setBottom( r.bottom() - m_sidePixmap.height() );
	p.drawTiledPixmap(r, m_sideTilePixmap );

	r = sideImageRect();
	r.setTop( r.bottom() - m_sidePixmap.height() );
	p.drawPixmap(r, m_sidePixmap);
}

void
KMenu::initialize()
{
	if (initialized()) return;

	// Load the configuration
	if (applet()) {
		m_config.drawSideImage  = applet()->getConfigValue<bool>(QStringLiteral("drawSideImage"));
		m_config.showTitles     = applet()->getConfigValue<bool>(QStringLiteral("showTitles"));
		m_config.showSearch     = applet()->getConfigValue<bool>(QStringLiteral("showSearch"));
		m_config.showRecentApps = applet()->getConfigValue<bool>(QStringLiteral("showRecentApps"));
		m_config.numRecentApps  = applet()->getConfigValue<int>(QStringLiteral("numRecentApps"));
		m_config.showRecentDocs = applet()->getConfigValue<bool>(QStringLiteral("showRecentDocs"));
		m_config.showSystem     = applet()->getConfigValue<bool>(QStringLiteral("showSystem"));	
		m_config.showSettings   = applet()->getConfigValue<bool>(QStringLiteral("showSettings"));
	}
	
	loadSidePixmap();
	setMargins();
		
	ServiceMenu::initialize();
	
	// Add the section headers
	if (m_config.showTitles) {
		addAction(new PopupMenuTitle(i18n("All Applications"), this));	
		m_applicationsAnchor = new PopupMenuTitle(i18n("Actions"), this);
		addAction(m_applicationsAnchor);
	} else {
		if (m_config.showRecentApps)
			addSeparator();
		m_applicationsAnchor = addSeparator();
	}

	// Add recent menu items and applications list
	if (m_config.showRecentApps)
		createRecentMenuItems();	
	createApplicationsItems();

	// Add actions section

	// NB: we do not need to worry about memory leaks here, since
	// our actionEvent handler automatically deletes menus associated with
	// any QAction that gets deleted
	if (m_config.showRecentDocs) {
		// Recent documents submenu
		auto *documentsMenu = new RecentDocsMenu(i18n("Recent Documents"), applet(), this);
		documentsMenu->setIcon(QIcon::fromTheme(QStringLiteral("document-open-recent")));
		addMenu(documentsMenu);
	}
	if (m_config.showSystem) {
		// My system submenu
		auto *systemMenu = new SystemMenu(i18n("My System"), applet(), this);
		systemMenu->setIcon(QIcon::fromTheme(QStringLiteral("computer")));
		addMenu(systemMenu);
	}
	if (m_config.showSettings) {
		// Settings submenu
	    auto *settingsMenu = new SettingsMenu(i18n("Settings"), applet(), this);
		settingsMenu->setIcon(QIcon::fromTheme(QStringLiteral("preferences-desktop")));
		addMenu(settingsMenu);
	}

	if (m_config.showRecentDocs || m_config.showSystem || m_config.showSettings)
		addSeparator();

	// Add the run command option
	QAction *action = addAction(QIcon::fromTheme(QStringLiteral("system-run")), i18n("Run Command..."));
	connect(action, &QAction::triggered, this, [](){invokeKRunner();});
	
	// Add the power/session options
	if (m_session.canSwitchUser() || m_session.canLock() || m_session.canLogout()) {
		addSeparator();
		if (m_session.canSwitchUser()) {
			action = addAction(QIcon::fromTheme(QStringLiteral("system-switch-user")), i18n("Switch User"));
			connect(action, &QAction::triggered, &m_session, &SessionManagement::switchUser);
		}
		if (m_session.canLock()) {
			action = addAction(QIcon::fromTheme(QStringLiteral("system-lock-screen")), i18n("Lock Session"));
			connect(action, &QAction::triggered, &m_session, &SessionManagement::lock);
		}
		if (m_session.canLogout()) {
			action = addAction(QIcon::fromTheme(QStringLiteral("system-log-out")), i18n("Log Out..."));
			connect(action, &QAction::triggered, &m_session, &SessionManagement::requestLogoutPrompt);
		}
	}

	setInitialized(true);
}

void
KMenu::reinitialize()
{
	if (!initialized())
		return;
	
	setInitialized(false);

	// Clear action lists
	m_recentActions.clear();
	m_applicationActions.clear();
	
	// Clear out the menu
	clear();

	// Clear out recent apps model
	if (m_recentApps) {
		m_recentApps->deleteLater();
		m_recentApps = nullptr;
	}

	m_applicationsAnchor = nullptr; // avoid dangling pointer

	// Reset the pixmaps, to save memory	
	m_sidePixmap = QPixmap();
	m_sideTilePixmap = QPixmap();

	// Finally, call initialize() again
	initialize();
}

void
KMenu::setMargins()
{
	if (m_config.drawSideImage) {
		setContentsMargins(
			layoutDirection() == Qt::LeftToRight ? m_sidePixmap.width() : 0,
			0,
			layoutDirection() == Qt::RightToLeft ? m_sidePixmap.width() : 0,
			0);
	} else {
		setContentsMargins(0, 0, 0, 0);
	}
}

void
KMenu::loadSidePixmap()
{
	// Reset the pixmaps
	m_sidePixmap = QPixmap();
	m_sideTilePixmap = QPixmap();
	
	if (!m_config.drawSideImage)
		return;

	// TODO: use better images (perhaps in SVG format?)
	QImage sideImage(QStringLiteral(":/qt/qml/plasma/applet/com/github/neeeeow/klassik/kmenu/kside.png"));
	if (sideImage.isNull() || sideImage.width() == 0 || sideImage.height() == 0)
		return;
	QImage sideTileImage(QStringLiteral(":/qt/qml/plasma/applet/com/github/neeeeow/klassik/kmenu/kside_tile.png"));
	if (sideTileImage.isNull() || sideTileImage.width() == 0 || sideTileImage.height() == 0)
		return;	

	// Colorize images and load in to pixmaps
	colorize(sideImage);	
	m_sidePixmap = QPixmap::fromImage(sideImage);
	colorize(sideTileImage);
	m_sideTilePixmap = QPixmap::fromImage(sideTileImage);

	// Create the painter for drawing the side text
	QPainter sidePainter(&m_sidePixmap);
	const QFont sideFont(QStringLiteral("Nimbus Sans"), 12, QFont::Bold); // TODO: use a better font
	sidePainter.setFont(sideFont);
	sidePainter.translate(0, m_sidePixmap.height());
	sidePainter.rotate(-90);

	// Area in which to draw the text
	const QRect textRect(5, 1, m_sidePixmap.height() - 5, m_sidePixmap.width() - 2);

	// Draw the text.
	const QString sideText = QStringLiteral("Klassik");
	sidePainter.setPen(Qt::black);
	sidePainter.drawText(textRect.translated(-1, 1), Qt::AlignVCenter | Qt::AlignLeft, sideText);
	sidePainter.setPen(Qt::white);
	sidePainter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, sideText);

	// pretile the pixmap to a height of at least 100 pixels
    if (m_sideTilePixmap.height() < 100)
    {
		int tiles = (int)(100 / m_sideTilePixmap.height()) + 1;
		QPixmap preTiledPixmap(m_sideTilePixmap.width(), m_sideTilePixmap.height() * tiles);
		preTiledPixmap.fill(Qt::transparent);
		QPainter tilePainter(&preTiledPixmap);
		tilePainter.drawTiledPixmap(preTiledPixmap.rect(), m_sideTilePixmap);
		tilePainter.end();
		m_sideTilePixmap = preTiledPixmap;
    }
}

QRect
KMenu::sideImageRect() const
{
	const int panelWidth = style()->pixelMetric(QStyle::PM_MenuPanelWidth, nullptr, this);
    const int hMargin = style()->pixelMetric(QStyle::PM_MenuHMargin, nullptr, this);
    const int vMargin = style()->pixelMetric(QStyle::PM_MenuVMargin, nullptr, this);

	// Rectangle containing our side pixmap
	const QRect pixRect(panelWidth + hMargin, panelWidth + vMargin,
						m_sidePixmap.width(), height() - 2 * (panelWidth + vMargin));

	// Convert to screen coordinates based on text direction
	return style()->visualRect(layoutDirection(), rect(), pixRect);
}

void
KMenu::createRecentMenuItems()
{	
	using namespace KActivities::Stats;
	using namespace KActivities::Stats::Terms;
	
	// Run our query once.
	const auto query = UsedResources
		| HighScoredFirst		
		| Agent::any()
		| Type::any()
		| Url::startsWith(QStringLiteral("applications:"))
		| Activity::current()
		| Limit(m_config.numRecentApps > 0 ? m_config.numRecentApps : 1);

	if (m_recentApps)
		m_recentApps->deleteLater();
	m_recentApps = new ResultModel(query, this);

	// Whenever an application is launched, update the recent apps list
	connectResultModel(m_recentApps, &KMenu::updateRecent);

	// Update the list once to initially populate it
	updateRecent();
}

void
KMenu::updateRecent()
{
	/* Updates the most used applications section. It probably *shouldn't* be called updateRecent,
	   but we follow the convention used by the original KDE 3 KMenu, even if it doesn't
	   make sense */
	
	cleanupActionList(m_recentActions);
	if (!m_recentApps)
		return;

	// Add the section header here so we can clear it if necessary
	if (m_config.showTitles) {
		PopupMenuTitle *recentHeader = new PopupMenuTitle(i18n("Most Used Applications"), this);
		m_recentActions.append(recentHeader);
	}

	QList<QAction *> actionList;
	for (int i=0; i < m_recentApps->rowCount(); ++i) {
		const QModelIndex index = m_recentApps->index(i,0);
		
		const QUrl resourceUrl(m_recentApps->data(index, KActivities::Stats::ResultModel::ResourceRole).toString());
		if (resourceUrl.scheme() != QStringLiteral("applications"))
			continue; // The resource url should always point to an application, but just to be safe
		const QString storageId = resourceUrl.path();
		const KService::Ptr service = KService::serviceByStorageId(storageId);
		QAction *action = createActionFromService(service);
		if (action)
			actionList.append(action);
	}

	if (actionList.isEmpty()) {
		QAction *emptyAction = new QAction(i18n("No Entries"), this);
		emptyAction->setEnabled(false);
		m_recentActions.append(emptyAction);	
	} else {	
		m_recentActions.append(actionList);
	}

	// Append the actions to the top of the menu (recent applications *always* go first)
	if (actions().isEmpty())
		addActions(m_recentActions);
	else
		insertActions(actions().constFirst(), m_recentActions);
}

void
KMenu::createApplicationsItems()
{	
	// Create the search bar container
	if (m_config.showSearch) {
		PopupMenuSearch *search = new PopupMenuSearch(i18n("Press '/' to search..."), Qt::Key_Slash, this);
		insertAction(m_applicationsAnchor, search);				
		if (QLineEdit *lineEdit = search->lineEdit()) {
			connect(this, &QMenu::aboutToHide, lineEdit, &QLineEdit::clear, Qt::UniqueConnection);
			connect(this, &QMenu::aboutToHide, lineEdit, &QLineEdit::clearFocus, Qt::UniqueConnection);
			connect(lineEdit, &QLineEdit::textChanged, this, &KMenu::updateSearchResults);
		}
	}
	
	connect(KSycoca::self(), &KSycoca::databaseChanged, this, &KMenu::updateApplications, Qt::UniqueConnection);
	updateApplications(); // Call the function to populate the menu itself
}

void
KMenu::updateSearchResults()
{
	QLineEdit *lineEdit = qobject_cast<QLineEdit *>(sender());
	if (!lineEdit)
		return;

	const QString text = lineEdit->text();
	
	auto setActionStates = [&](this auto&& self, QAction *parent, const QList<QAction *> &children) -> bool {
		bool enableParent = false;
			
		for (QAction *action : children) {
			if (QMenu *subMenu = action->menu()) {
				// If the action is a sub menu, recurse through it and check to see if the current action (which
				// opens a sub menu) must be enabled, if so, set enableParent to true for now.
			    enableParent |= self(action, subMenu->actions());
			} else {					
				if (text.isEmpty() || (action->text().left(text.length()).compare(text, Qt::CaseInsensitive) == 0)) {
					// Item must be enabled either if the search string matches the action name, or if the search bar is empty
					enableParent = true;
					action->setEnabled(true);
				} else
					action->setEnabled(false);
			}
		}

		if (parent)
			parent->setEnabled(enableParent);

		return enableParent;
	};

	setActionStates(nullptr, m_applicationActions);
}

void
KMenu::updateApplications()
{
	cleanupActionList(m_applicationActions);
	
	// The root of the applications menu
    const KServiceGroup::Ptr root = KServiceGroup::root();

	const QList<QAction *> actionList = createActionsFromServiceGroup(root);

	if (actionList.isEmpty()) {
		QAction *emptyAction = new QAction(i18n("No Entries"), this);
		emptyAction->setEnabled(false);
		m_applicationActions.append(emptyAction);
	} else {
		m_applicationActions.append(actionList);		
	}

	// If m_applicationsAnchor is null, insertActions simply appends
	// the actions to the menu. This behaviour is fine (although
	// it should never really be null when updateApplications
	// is called).
	insertActions(m_applicationsAnchor, m_applicationActions);
	
}

/* Mouse events adapted from KDE 3.5 kicker source code.
   Copyright (c) 1996-2000 the KDE 3 kicker authors.
   Source available: https://kde.org/info/1-2-3/3.5.10/ */

std::unique_ptr<QMouseEvent>
KMenu::translateMouseEvent( QMouseEvent* e )
{
	if (!m_config.drawSideImage)
		return nullptr;
	
    const QRect side = sideImageRect();

	if (!side.contains(e->position().toPoint()))
		return nullptr;

	QPointF newpos( e->position() );
	layoutDirection() == Qt::RightToLeft ?
		newpos.setX( newpos.x() - side.width() ) :
		newpos.setX( newpos.x() + side.width() );
	QPointF newglobal( e->globalPosition() );
	layoutDirection() == Qt::RightToLeft ?
		newglobal.setX( newglobal.x() - side.width() ) :
		newglobal.setX( newglobal.x() + side.width() );
	
	return std::make_unique<QMouseEvent>(
		e->type(), newpos, newglobal, e->button(), e->buttons(), e->modifiers());
}

void
KMenu::mousePressEvent(QMouseEvent * e)
{
    auto newEvent(translateMouseEvent(e));
    ServiceMenu::mousePressEvent( newEvent ? newEvent.get() : e );
}

void
KMenu::mouseReleaseEvent(QMouseEvent *e)
{
    auto newEvent(translateMouseEvent(e));
    ServiceMenu::mouseReleaseEvent( newEvent ? newEvent.get() : e );
}

void
KMenu::mouseMoveEvent(QMouseEvent *e)
{
    auto newEvent(translateMouseEvent(e));
    ServiceMenu::mouseMoveEvent( newEvent ? newEvent.get() : e );
}

/* Colorize code adapted from KDE 3 code.
   Copyright (c) 1996-2000 the KDE 3 authors.
   Source available: https://kde.org/info/1-2-3/3.5.10/ */

void
KMenu::colorize(QImage &image) const
{
	QColor color;
	
	KColorScheme activeScheme(QPalette::Active, KColorScheme::Selection);
	QColor activeTitle = activeScheme.background().color();

	KColorScheme inactiveScheme(QPalette::Inactive, KColorScheme::Selection);
	QColor inactiveTitle = inactiveScheme.background().color();

	// figure out which color is most suitable for recoloring to
    int h1, s1, v1, h2, s2, v2, h3, s3, v3;
    activeTitle.getHsv(&h1, &s1, &v1);
    inactiveTitle.getHsv(&h2, &s2, &v2);
    palette().color(QPalette::Active, QPalette::Window).getHsv(&h3, &s3, &v3);

	if ( (qAbs(h1-h3)+qAbs(s1-s3)+qAbs(v1-v3) < qAbs(h2-h3)+qAbs(s2-s3)+qAbs(v2-v3)) &&
		 ((qAbs(h1-h3)+qAbs(s1-s3)+qAbs(v1-v3) < 32) || (s1 < 32)) && (s2 > s1))
		color = inactiveTitle;
	else
		color = activeTitle;

	int r, g, b;
	color.getRgb(&r, &g, &b);
	int gray = qGray(r, g, b);
	if (gray > 180) {
		r = (r - (gray - 180) < 0 ? 0 : r - (gray - 180));
		g = (g - (gray - 180) < 0 ? 0 : g - (gray - 180));
		b = (b - (gray - 180) < 0 ? 0 : b - (gray - 180));
	} else if (gray < 76) {
		r = (r + (76 - gray) > 255 ? 255 : r + (76 - gray));
		g = (g + (76 - gray) > 255 ? 255 : g + (76 - gray));
        b = (b + (76 - gray) > 255 ? 255 : b + (76 - gray));
	}
	color.setRgb(r, g, b);

	// convert the image, just in case
	if (image.format() != QImage::Format_ARGB32) {
		image = image.convertToFormat(QImage::Format_ARGB32);
	}

	int rval, gval, bval, val, alpha;
    float rcol = color.red(), gcol = color.green(), bcol = color.blue();

	for (int y = 0; y < image.height(); ++y) {
		QRgb *line = reinterpret_cast<QRgb*>(image.scanLine(y));
		for (int x = 0; x < image.width(); ++x) {
			val = qGray(line[x]);
			if (val < 128) {
				rval = static_cast<int>(rcol/128*val);
				gval = static_cast<int>(gcol/128*val);
				bval = static_cast<int>(bcol/128*val);
			}
			else if (val > 128) {
				rval = static_cast<int>((val-128)*(2-rcol/128)+rcol-1);
				gval = static_cast<int>((val-128)*(2-gcol/128)+gcol-1);
				bval = static_cast<int>((val-128)*(2-bcol/128)+bcol-1);
			}
			else { // val == 128
				rval = static_cast<int>(rcol);
				gval = static_cast<int>(gcol);
				bval = static_cast<int>(bcol);
			}

			alpha = qAlpha(line[x]);
			line[x] = qRgba(rval, gval, bval, alpha);
		}
	}
}
