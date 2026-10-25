/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "panelcontainment.h"

#include <QGuiApplication>
#include <QUrl>
#include <QMimeData>

#include <KPluginFactory>
#include <KUrlMimeData>

K_PLUGIN_CLASS_WITH_JSON(PanelContainment, "metadata.json")

PanelContainment::PanelContainment(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args)
: Plasma::Containment(parentObject, data, args),
	m_quickPalette(new QQuickPalette(this))
{
	loadPalette();
	qGuiApp->installEventFilter(this);
	connect(this, &PanelContainment::colorizeChanged, this, &PanelContainment::loadPalette);
}

bool
PanelContainment::eventFilter(QObject *watched, QEvent *event)
{
	if (watched == qGuiApp && event->type() == QEvent::ApplicationPaletteChange)
		loadPalette();
	return Plasma::Containment::eventFilter(watched, event);
}

bool
PanelContainment::processMimeData(QMimeData *mimeData, int x, int y)
{
	if (!mimeData)
		return false;

	// We don't deal with applets being dragged/dropped - hand that off to ContainmentItem
	if (mimeData->hasFormat(QStringLiteral("text/x-plasmoidinstanceid"))
		|| mimeData->hasFormat(QStringLiteral("text/x-plasmoidservicename")))
		return false;

	// URLs to append to the panel
	QList<QUrl> urls;
	if (mimeData->hasUrls())
		urls = KUrlMimeData::urlsFromMimeData(mimeData);
	else if (mimeData->hasFormat(QStringLiteral("text/x-orgkdeplasmataskmanager_taskurl")))
		urls = {QUrl(QString::fromUtf8(mimeData->data(QStringLiteral("text/x-orgkdeplasmataskmanager_taskurl"))))};
	urls.removeIf([](const QUrl &u) { return !u.isValid() || u.isEmpty(); });
	if (urls.isEmpty())
		return false;

	// Add the icons to the panel
	for (const QUrl &url : std::as_const(urls)) {
		createApplet(QStringLiteral("com.github.neeeeow.klassik.panel.icon"),
					 QVariantList() << url,
					 QRectF(x, y, -1, -1));
	}

	return true;

}

void
PanelContainment::loadPalette()
{
	if (!m_quickPalette)
		m_quickPalette = new QQuickPalette(this);
	
	QPalette palette = QGuiApplication::palette();

	// Colorize the palette
	if (m_colorize) {
		const QPalette::ColorGroup groups[] = {
			QPalette::Active,
			QPalette::Inactive,
			QPalette::Disabled
		};
		for (QPalette::ColorGroup group : groups) {
			// Base colors
			const QColor buttonColor = tintColor(palette.color(group, QPalette::Button));
			const QColor windowColor = tintColor(palette.color(group, QPalette::Window));

			// Derive necessary brushes
			const QBrush window = QBrush(windowColor);
			const QBrush button = QBrush(buttonColor);			
			const QBrush dark = QBrush(buttonColor.darker());
			const QBrush mid = QBrush(buttonColor.darker(150));
			const QBrush light = QBrush(buttonColor.lighter(150));

			// Other brushes (taken directly from the palette)
			const QBrush windowText = palette.brush(group, QPalette::WindowText);
			const QBrush text = palette.brush(group, QPalette::Text);
			const QBrush bright_text = palette.brush(group, QPalette::BrightText);
			const QBrush base = palette.brush(group, QPalette::Base);

			// Set the color group of the palette
			palette.setColorGroup(group, windowText, button, light, dark, mid, text, bright_text, base, window);			
		}
	}

	// Loads in the QPalette
	m_quickPalette->fromQPalette(palette);

	// Notify that the palette has been changed
	Q_EMIT paletteChanged();
}

QColor
PanelContainment::tintColor(const QColor &base)
{
	QColor color;
	
	const QPalette palette = QGuiApplication::palette();
	const QColor activeTitle = palette.color(QPalette::Active, QPalette::Highlight);
	const QColor inactiveTitle = palette.color(QPalette::Inactive, QPalette::Highlight);

	// figure out which color is most suitable for recoloring to
	int h1, s1, v1, h2, s2, v2, h3, s3, v3;
	activeTitle.getHsv(&h1, &s1, &v1);
	inactiveTitle.getHsv(&h2, &s2, &v2);
    base.getHsv(&h3, &s3, &v3);

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

	int val = qGray(base.rgb());
	int rval, gval, bval;
	float rcol = color.red(), gcol = color.green(), bcol = color.blue();

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

	return QColor(rval, gval, bval, base.alpha());
}

#include "panelcontainment.moc"
