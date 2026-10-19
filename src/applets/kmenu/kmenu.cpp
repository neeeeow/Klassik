/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "kmenu.h"
#include "servicemenuapplet.h"
#include "kmenumenutitle.h"
#include "kmenumenusearch.h"
#include "recentdocsmenu.h"
#include "settingsmenu.h"
#include "systemmenu.h"

#include <QPainter>
#include <QEvent>
#include <QAction>
#include <QLineEdit>
#include <QRect>
#include <QPaintEvent>

#include <KLocalizedString>
#include <KSycoca>

#include <PlasmaActivities/Stats/ResultSet>
#include <PlasmaActivities/Stats/Query>

#include <sessionmanagement.h>

KMenu::KMenu(ServiceMenuApplet *applet, QWidget *parent)
	: ServiceGroupMenu(applet, parent),
	  m_session(new SessionManagement(this)) {}

void
KMenu::changeEvent(QEvent *e)
{
	if (e->type() == QEvent::PaletteChange && !m_sidePixmap.isNull() && !m_sideTilePixmap.isNull()) {
		loadSidePixmap();
		update();
	}
	if (e->type() == QEvent::LayoutDirectionChange) {
		setMargins();
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

	QRect r = sideImageRect();
	r.setBottom( r.bottom() - m_sidePixmap.height() );
	p.drawTiledPixmap(r, m_sideTilePixmap );

	r = sideImageRect();
	r.setTop( r.bottom() - m_sidePixmap.height() + 1 );
	p.drawPixmap(r, m_sidePixmap);
}

void
KMenu::rebuild()
{	
	// Load the configuration
	if (applet()) {
		m_config.drawSideImage	= applet()->getConfigValue<bool>(QStringLiteral("drawSideImage"));
		m_config.showTitles		= applet()->getConfigValue<bool>(QStringLiteral("showTitles"));
		m_config.showSearch		= applet()->getConfigValue<bool>(QStringLiteral("showSearch"));
		m_config.showMostUsedApps = applet()->getConfigValue<bool>(QStringLiteral("showRecentApps"));
		m_config.numMostUsedApps	= applet()->getConfigValue<int>(QStringLiteral("numRecentApps"));
		m_config.showRecentDocs = applet()->getConfigValue<bool>(QStringLiteral("showRecentDocs"));
		m_config.showSystem		= applet()->getConfigValue<bool>(QStringLiteral("showSystem"));	
		m_config.showSettings	= applet()->getConfigValue<bool>(QStringLiteral("showSettings"));
	}

	// Load in pixmaps and set margins
	loadSidePixmap();
	setMargins();
}

void
KMenu::populate()
{
	m_applicationActions.clear();
	resetSearchFilter();
	
	// Most used applications section
	if (m_config.showMostUsedApps) {
		if (m_config.showTitles)
			addAction(new KMenuTitle(i18n("Most Used Applications"), this));
		addActions(mostUsedAppsActions());
		if (!m_config.showTitles)
			addSeparator();
	}

	if (m_config.showTitles)
		addAction(new KMenuTitle(i18n("All Applications"), this));		

	// All applications search bar
	if (m_config.showSearch) {
		auto *search = new KMenuSearch(i18n("Press '/' to search..."), Qt::Key_Slash, this);
		addAction(search);				
		if (QLineEdit *lineEdit = search->lineEdit())	
			connect(lineEdit, &QLineEdit::textChanged, this, &ServiceGroupMenu::setSearchFilter);
	}

	// All applications entries
	const QList<QAction *> appActions = createActionsFromServiceGroup(KServiceGroup::root());		
	if (appActions.isEmpty()) {
		QAction *emptyAction = addAction(i18n("No Entries"));
		emptyAction->setEnabled(false);
	} else {
		m_applicationActions = QList<QPointer<QAction>>(appActions.begin(), appActions.end());
		addActions(applicationActions());
	}

	// Actions section
	if (m_config.showTitles)
		addAction(new KMenuTitle(i18n("Actions"), this));
	else
		addSeparator();

	// NB: we do not need to worry about memory leaks here, since
	// our actionEvent handler automatically deletes menus associated with
	// any QAction that gets deleted
	if (m_config.showRecentDocs) {
		// Recent documents submenu
		auto *documentsMenu = new RecentDocsMenu(QIcon::fromTheme(QStringLiteral("document-open-recent")),
												 i18n("Recent Documents"), applet(), this);
		addMenu(documentsMenu);
	}
	if (m_config.showSystem) {
		// My system submenu
		auto *systemMenu = new SystemMenu(QIcon::fromTheme(QStringLiteral("computer")),
										  i18n("My System"), applet(), this);
		addMenu(systemMenu);
	}
	if (m_config.showSettings) {
		// Settings submenu
		auto *settingsMenu = new SettingsMenu(QIcon::fromTheme(QStringLiteral("preferences-desktop")),
											  i18n("Settings"), applet(), this);
		addMenu(settingsMenu);
	}

	if (m_config.showRecentDocs || m_config.showSystem || m_config.showSettings)
		addSeparator();

	// Add the run command option
	QAction *runAction = addAction(QIcon::fromTheme(QStringLiteral("system-run")), i18n("Run Command…"));
	connect(runAction, &QAction::triggered, this, [](){invokeKRunner();});

	if (m_session->canSwitchUser() || m_session->canLock() || m_session->canLogout()) {
		addSeparator();
		QAction *action;
		if (m_session->canSwitchUser()) {
			action = addAction(QIcon::fromTheme(QStringLiteral("system-switch-user")), i18n("Switch User"));
			connect(action, &QAction::triggered, m_session, &SessionManagement::switchUser);
		}
		if (m_session->canLock()) {
			action = addAction(QIcon::fromTheme(QStringLiteral("system-lock-screen")), i18n("Lock Session"));
			connect(action, &QAction::triggered, m_session, &SessionManagement::lock);
		}
		if (m_session->canLogout()) {
			action = addAction(QIcon::fromTheme(QStringLiteral("system-log-out")), i18n("Log Out…"));
			connect(action, &QAction::triggered, m_session, &SessionManagement::requestLogoutPrompt);
		}
	}	
}

void
KMenu::setMargins()
{
	if (m_config.drawSideImage && (!m_sidePixmap.isNull())) {
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
	if (sideImage.isNull())
		return;
	QImage sideTileImage(QStringLiteral(":/qt/qml/plasma/applet/com/github/neeeeow/klassik/kmenu/kside_tile.png"));
	if (sideTileImage.isNull())
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
		const int tiles = (int)(100 / m_sideTilePixmap.height()) + 1;
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

QList<QAction *>
KMenu::mostUsedAppsActions()
{
	QList<QAction *> actions;
	
	using namespace KActivities::Stats;
	using namespace KActivities::Stats::Terms;	
	
	// Create the query used for tracking the most used applications
	const auto query = UsedResources
		| HighScoredFirst		
		| Agent::any()
		| Type::any()
		| Url::startsWith(QStringLiteral("applications:"))
		| Activity::current();

	ResultSet results(query);
	ResultSet::const_iterator resultIt;
    resultIt = results.begin();
    while (actions.count() < m_config.numMostUsedApps && resultIt != results.end()) {
		const QString resource = (*resultIt).resource();
		++resultIt;

		const QUrl resourceUrl(resource);
		if (resourceUrl.scheme() != QStringLiteral("applications"))
			continue; // The resource url should always point to an application, but just to be safe
		const KService::Ptr service = KService::serviceByStorageId(resourceUrl.path());
		if (!service || service->noDisplay())
			continue;
		if (QAction *action = createActionFromService(service))
			actions.append(action);
	}

	if (actions.isEmpty()) {
		auto *emptyAction = new QAction(i18n("No Entries"), this);
		emptyAction->setEnabled(false);
		actions.append(emptyAction);
	}

	return actions;
}

void
KMenu::applySearchFilter()
{
	if (!initialized() || isEmpty() || !isVisible())
		return;

	for (QAction *action : applicationActions()) {
		// Set the enabled/disabled state of the action
		filterAction(action);
	}
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
	
	QColor activeTitle = palette().color(QPalette::Active, QPalette::Highlight);
	QColor inactiveTitle = palette().color(QPalette::Inactive, QPalette::Highlight);

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
