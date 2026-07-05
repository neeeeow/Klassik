#include "kmenu.h"
#include "popupmenutitle.h"
#include "popupmenusearch.h"
#include "recentdocsmenu.h"
#include "settingsmenu.h"

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

bool
KMenu::event(QEvent *e)
{
	if (e->type() == QEvent::DevicePixelRatioChange) {
		setMargins();
	}
	return ServiceMenu::event(e);
}

void
KMenu::changeEvent(QEvent *e)
{
	if (e->type() == QEvent::PaletteChange && !m_sidePixmap.isNull() && !m_sideTilePixmap.isNull()) {
		loadSidePixmap();
	}
	ServiceMenu::changeEvent(e);
}

void
KMenu::paintEvent(QPaintEvent *e)
{
	ServiceMenu::paintEvent(e);
	if (m_sidePixmap.isNull() || m_sideTilePixmap.isNull() || !applet()->getConfigValue<bool>(QStringLiteral("drawSideImage")))
		return;

	const qreal dpr = devicePixelRatio();

	QPainter p(this);
	p.scale(1/dpr, 1/dpr);

	QRect r = getScaledRect(sideImageRect(), dpr);
	r.setBottom( r.bottom() - m_sidePixmap.height() );
	p.drawTiledPixmap(r, m_sideTilePixmap );

	r = getScaledRect(sideImageRect(), dpr);
	r.setTop( r.bottom() - m_sidePixmap.height() );
	p.drawPixmap(r, m_sidePixmap);
}

void
KMenu::initialize()
{
	if (initialized()) return;
	
	loadSidePixmap();
	setMargins();
		
	ServiceMenu::initialize();
	
	// Add the section headers

	if (applet()->getConfigValue<bool>(QStringLiteral("showTitles"))) {
		addAction(new PopupMenuTitle(i18n("All Applications"), this));	
		m_applicationsAnchor = new PopupMenuTitle(i18n("Actions"), this);
		addAction(m_applicationsAnchor);
	} else {
		if (applet()->getConfigValue<bool>(QStringLiteral("showRecentApps")))
			addSeparator();
		m_applicationsAnchor = addSeparator();
	}

	// Add recent menu items and applications list
	if (applet()->getConfigValue<bool>(QStringLiteral("showRecentApps")))
		createRecentMenuItems();	
	createApplicationsItems();

	// Add actions section

	if (applet()->getConfigValue<bool>(QStringLiteral("showSettings"))) {
		// Settings submenu
		SettingsMenu *settingsMenu = new SettingsMenu(i18n("Settings"), applet(), this);
		settingsMenu->setIcon(QIcon::fromTheme(QStringLiteral("preferences-desktop")));
		addMenu(settingsMenu);
	}

	if (applet()->getConfigValue<bool>(QStringLiteral("showRecentDocs"))) {
		// Recent documents submenu
		RecentDocsMenu *documentsMenu = new RecentDocsMenu(i18n("Recent Documents"), applet(), this);
		documentsMenu->setIcon(QIcon::fromTheme(QStringLiteral("document-open-recent")));
		addMenu(documentsMenu);
	}

	if (applet()->getConfigValue<bool>(QStringLiteral("showSettings")) ||
		applet()->getConfigValue<bool>(QStringLiteral("showRecentDocs")))
		addSeparator();

	// Add the run command option
	QAction *action = addAction(QIcon::fromTheme(QStringLiteral("system-run")), i18n("Run Command..."));
	connect(action, &QAction::triggered, this, [](){invokeKRunner();});
	
	// Add the power/session options
	addSeparator();
	if (m_session.canSwitchUser()) {
		action = addAction(QIcon::fromTheme(QStringLiteral("system-switch-user")), i18n("Switch User"));
		connect(action, &QAction::triggered, &m_session, &SessionManagement::switchUser);
	}
	if (m_session.canLock()) {
		action = addAction(QIcon::fromTheme(QStringLiteral("system-lock-screen")), i18n("Lock"));
		connect(action, &QAction::triggered, &m_session, &SessionManagement::lock);
	}
	if (m_session.canLogout()) {
		action = addAction(QIcon::fromTheme(QStringLiteral("system-log-out")), i18n("Log Out..."));
		connect(action, &QAction::triggered, &m_session, &SessionManagement::requestLogoutPrompt);
	}	

	setInitialized(true);
}

void
KMenu::reinitialize()
{
	if (!initialized())
		return;
	
	setInitialized(false);

	// Clear out the menu
	QList<QAction *> allActions = actions();
	cleanupActionList(allActions);
	clear();

	// Clear action lists
	m_recentActions.clear();
	m_applicationActions.clear();

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
	if (applet()->getConfigValue<bool>(QStringLiteral("drawSideImage"))) {
		const qreal dpr = devicePixelRatio();	
		setContentsMargins(
			layoutDirection() == Qt::LeftToRight ? qCeil(m_sidePixmap.width() / dpr) : 0,
			0,
			layoutDirection() == Qt::RightToLeft ? qCeil(m_sidePixmap.width() / dpr): 0,
			0);
	} else {
		setContentsMargins(0, 0, 0, 0);
	}

	adjustSize();
}

void
KMenu::loadSidePixmap()
{
	if (!applet()->getConfigValue<bool>(QStringLiteral("drawSideImage")))
		return;
	
	QImage image;
	image.load(QStringLiteral(":/qt/qml/plasma/applet/com/github/neeeeow/klassik/kmenu/kside.png"));
	if (image.isNull())
		return;
	colorize(image);
	m_sidePixmap = QPixmap::fromImage(image);

	// Create the painter for drawing the side text
	QPainter sidePainter(&m_sidePixmap);
	QFont sideFont(QStringLiteral("Nimbus Sans"), 12, QFont::Bold); // TODO: use a better font
	sidePainter.setFont(sideFont);
	sidePainter.translate(0, m_sidePixmap.height());
	sidePainter.rotate(-90);

	// Area in which to draw the text
	QRect textRect(5, 1, m_sidePixmap.height() - 5, m_sidePixmap.width() - 2);

	// Draw the text. The KDE version is queried at compile-time, meaning the menu
	// must be recompiled whenever plasma is updated to emsure this is accurate.
	QString sideText = QStringLiteral("KDE %1.%2").arg(PLASMA_VERSION_MAJOR).arg(PLASMA_VERSION_MINOR);
	sidePainter.setPen(Qt::black);
	sidePainter.drawText(textRect.translated(-1, 1), Qt::AlignVCenter | Qt::AlignLeft, sideText);
	sidePainter.setPen(Qt::white);
	sidePainter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, sideText);

	image.load(QStringLiteral(":/qt/qml/plasma/applet/com/github/neeeeow/klassik/kmenu/kside_tile.png"));
	if (image.isNull())
		return;
	colorize(image);
	m_sideTilePixmap = QPixmap::fromImage(image);

	// pretile the pixmap to a height of at least 100 pixels
    if (m_sideTilePixmap.height() < 100)
    {
		int tiles = (int)(100 / m_sideTilePixmap.height()) + 1;
		QPixmap preTiledPixmap(m_sideTilePixmap.width(), m_sideTilePixmap.height() * tiles);
		QPainter tilePainter(&preTiledPixmap);
		tilePainter.drawTiledPixmap(preTiledPixmap.rect(), m_sideTilePixmap);
		m_sideTilePixmap = preTiledPixmap;
    }
	
	return;
}

QRect
KMenu::sideImageRect()
{
	int panelWidth = style()->pixelMetric(QStyle::PM_MenuPanelWidth, nullptr, this);
    int hMargin = style()->pixelMetric(QStyle::PM_MenuHMargin, nullptr, this);
    int vMargin = style()->pixelMetric(QStyle::PM_MenuVMargin, nullptr, this);

	// Rectangle containing our side pixmap
	QRect pixRect(panelWidth + hMargin, panelWidth + vMargin,
				  m_sidePixmap.width() / devicePixelRatio(), height() - 2 * (panelWidth + vMargin));

	// Convert to screen coordinates based on text direction
	return style()->visualRect(layoutDirection(), rect(), pixRect);
}

void
KMenu::colorize(QImage &image)
{
	/* Taken and adapted from KDE 3.5.10 source */
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
	if (image.format() != QImage::Format_ARGB32_Premultiplied) {
		image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
	}
	
	int pixels = image.width() * image.height();
	QRgb *data = reinterpret_cast<QRgb*>(image.bits());

	int rval, gval, bval, val, alpha;
    float rcol = color.red(), gcol = color.green(), bcol = color.blue();

	for (int i = 0; i < pixels; ++i) {
		val = qGray(data[i]);
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

		alpha = qAlpha(data[i]);
		data[i] = qRgba(rval, gval, bval, alpha);
	}
}

void
KMenu::createRecentMenuItems()
{	
	using namespace KActivities::Stats;
	using namespace KActivities::Stats::Terms;
	
	// Run our query once.
	auto query = UsedResources
		| HighScoredFirst		
		| Agent::any()
		| Type::any()
		| Url::startsWith(QStringLiteral("applications:"))
		| Activity::current()
		| Limit(applet()->getConfigValue<int>(QStringLiteral("numRecentApps")));

	if (m_recentApps) {
		m_recentApps->deleteLater();
		m_recentApps = nullptr;
	}
	m_recentApps = new ResultModel(query, this);

	// Whenever an application is launched, update the recent apps list
	connect(m_recentApps, &ResultModel::dataChanged, this, &KMenu::updateRecent);
	connect(m_recentApps, &ResultModel::modelReset, this, &KMenu::updateRecent);
	connect(m_recentApps, &ResultModel::rowsInserted, this, &KMenu::updateRecent);
	connect(m_recentApps, &ResultModel::rowsMoved, this, &KMenu::updateRecent);
	connect(m_recentApps, &ResultModel::rowsRemoved, this, &KMenu::updateRecent);

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
	if (applet()->getConfigValue<bool>(QStringLiteral("showTitles"))) {
		PopupMenuTitle *recentHeader = new PopupMenuTitle(i18n("Most Used Applications"), this);
		m_recentActions.append(recentHeader);
	}

	QList<QAction *> actionList;
	for (int i=0; i < m_recentApps->rowCount(); ++i) {
		QModelIndex index = m_recentApps->index(i,0);
		
		const QUrl resourceUrl(m_recentApps->data(index, KActivities::Stats::ResultModel::ResourceRole).toString());
		if (resourceUrl.scheme() != QStringLiteral("applications"))
			continue; // The resource url should always point to an application, but just to be safe
		const QString storageId = resourceUrl.path();
		KService::Ptr service = KService::serviceByStorageId(storageId);
		QAction *action = createActionFromService(service);
		if (action)
			actionList.append(action);
	}

	if (actionList.isEmpty()) {
		QAction *emptyAction = new QAction(i18n("No Entries"), this);
		emptyAction->setEnabled(false);
		m_recentActions.append(emptyAction);	
	} else {	
		for (QAction *action : actionList) {
			m_recentActions.append(action);		
		}
	}

	if (QAction *anchor = actions().first())
		insertActions(anchor, m_recentActions);
	else
		addActions(m_recentActions);
}

void
KMenu::createApplicationsItems()
{	
	// Create the search bar container
	if (applet()->getConfigValue<bool>(QStringLiteral("showSearch"))) {
		PopupMenuSearch *search = new PopupMenuSearch(i18n("Press '/' to search..."), Qt::Key_Slash, this);
		insertAction(m_applicationsAnchor, search);				
		if (QLineEdit *lineEdit = search->lineEdit()) {
			connect(this, &QMenu::aboutToHide, lineEdit, &QLineEdit::clear, Qt::UniqueConnection);
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

	QString text = lineEdit->text();

	auto setActionStates = [&](this auto&& self, QAction *parent, QList<QAction *> children) -> bool {
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
    KServiceGroup::Ptr root = KServiceGroup::root();

	QList<QAction *> actionList = actionListFromServiceGroup(root);

	if (actionList.isEmpty()) {
		QAction *emptyAction = new QAction(i18n("No Entries"), this);
		emptyAction->setEnabled(false);
		m_applicationActions.append(emptyAction);
	} else {
		for (QAction *action : actionList)
			m_applicationActions.append(action);		
	}

	insertActions(m_applicationsAnchor, m_applicationActions);
	
}

/* Mouse events adapted from KDE 3.5 kicker source code.
   Copyright (c) 1996-2000 the KDE 3 kicker authors.
   Source available: https://kde.org/info/1-2-3/3.5.10/ */

QMouseEvent*
KMenu::translateMouseEvent( QMouseEvent* e )
{
	if (!applet()->getConfigValue<bool>(QStringLiteral("drawSideImage")))
		return e->clone();
	
    QRect side = sideImageRect();

	if (!side.contains(e->position().toPoint()))
		return e->clone();

	QPointF newpos( e->position() );
	layoutDirection() == Qt::RightToLeft ?
		newpos.setX( newpos.x() - side.width() ) :
		newpos.setX( newpos.x() + side.width() );
	QPointF newglobal( e->globalPosition() );
	layoutDirection() == Qt::RightToLeft ?
		newglobal.setX( newpos.x() - side.width() ) :
		newglobal.setX( newpos.x() + side.width() );
	
	return new QMouseEvent(e->type(), newpos, newglobal, e->button(), e->buttons(), e->modifiers());
}

void
KMenu::mousePressEvent(QMouseEvent * e)
{
	// use smart pointers here to avoid memory leaks
	std::unique_ptr<QMouseEvent> newEvent(translateMouseEvent(e));
    ServiceMenu::mousePressEvent( newEvent.get() );
}

void
KMenu::mouseReleaseEvent(QMouseEvent *e)
{
    std::unique_ptr<QMouseEvent> newEvent(translateMouseEvent(e));
    ServiceMenu::mouseReleaseEvent( newEvent.get() );
}

void
KMenu::mouseMoveEvent(QMouseEvent *e)
{
    std::unique_ptr<QMouseEvent> newEvent(translateMouseEvent(e));
    ServiceMenu::mouseMoveEvent( newEvent.get() );
}
