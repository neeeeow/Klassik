/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "helpmenu.h"
#include "aboutdialog.h"

#include <QGuiApplication>

#include <KLocalizedString>
#include <KPluginMetaData>
#include <KStandardActions>
#include <KBugReport>
#include <KAboutApplicationDialog>

HelpMenu::HelpMenu(QWidget *parent)
	: HelpMenu(KAboutData::applicationData(), parent) {}

HelpMenu::HelpMenu(const KPluginMetaData &plugin, QWidget *parent)
	: HelpMenu(aboutDataFromMetaData(plugin), parent) {}

HelpMenu::HelpMenu(const KAboutData &aboutData, QWidget *parent)
	: QMenu(i18n("Help"), parent), m_aboutData(aboutData)
{
	// Standard help icon
	setIcon(QIcon::fromTheme(QStringLiteral("help-contents")));

	// Application name and icon
	const QString displayName = displayNameFromAboutData(aboutData);
	const QIcon icon = iconFromAboutData(aboutData);

	// Report bug action
	addAction(KStandardActions::reportBug(this, &HelpMenu::reportBug, this));
	addSeparator();

	// About application section
	QAction *aboutAppAction = addAction(icon, i18n("&About %1", QString(displayName).replace(QLatin1Char('&'),
																							 QStringLiteral("&&"))));
	connect(aboutAppAction, &QAction::triggered, this, &HelpMenu::aboutApplication);

	// About Klassik action
	QAction *aboutKlassikAction = addAction(QIcon::fromTheme(QStringLiteral("help-about")),
											i18n("About &Klassik"));
	connect(aboutKlassikAction, &QAction::triggered, this, &HelpMenu::aboutKlassik);
	
}

void
HelpMenu::aboutApplication()
{
	if (!m_aboutApplication) {
		m_aboutApplication = new KAboutApplicationDialog(m_aboutData, parentWidget());
		m_aboutApplication->setAttribute(Qt::WA_DeleteOnClose);
	}
	m_aboutApplication->show();
	m_aboutApplication->raise();
	m_aboutApplication->activateWindow();	
}

void
HelpMenu::aboutKlassik()
{
	if (!m_aboutDialog) {
		m_aboutDialog = new AboutDialog(parentWidget());
		m_aboutDialog->setAttribute(Qt::WA_DeleteOnClose);
	}
	m_aboutDialog->show();
	m_aboutDialog->raise();
	m_aboutDialog->activateWindow();
}

void
HelpMenu::reportBug()
{
	if (!m_bugReport) {
		m_bugReport = new KBugReport(m_aboutData, parentWidget());
		m_bugReport->setAttribute(Qt::WA_DeleteOnClose);
	}
	m_bugReport->show();
	m_bugReport->raise();
	m_bugReport->activateWindow();
}

KAboutData
HelpMenu::aboutDataFromMetaData(const KPluginMetaData &plugin)
{
	KAboutData ret(plugin.pluginId(),
				   plugin.name(),
				   plugin.version(),
				   plugin.description(),
				   !plugin.license().isEmpty() ? KAboutLicense::byKeyword(plugin.license()).key() : KAboutLicense::Unknown,
				   plugin.copyrightText(),
				   QString(),
				   plugin.website(),
				   plugin.bugReportUrl());

	ret.setProgramLogo(QIcon::fromTheme(plugin.iconName(),
										QIcon::fromTheme(QStringLiteral("help-about"))));

	for (const KAboutPerson &author : plugin.authors())
		ret.addAuthor(author);

	for (const KAboutPerson &credit : plugin.otherContributors())
		ret.addCredit(credit);

	// We need to manually build the translator list
	QStringList names;
	QStringList emails;
	for (const KAboutPerson &translator : plugin.translators()) {
		names.append(translator.name());
		emails.append(translator.emailAddress());
	}
	if (!names.empty())
		ret.setTranslator(names.join(QLatin1Char(',')), emails.join(QLatin1Char(',')));
	
	return ret;
}

QString
HelpMenu::displayNameFromAboutData(const KAboutData &aboutData)
{	
	QString displayName = aboutData.displayName();
	if (displayName.isEmpty())
		displayName = QGuiApplication::applicationDisplayName();
	if (displayName.isEmpty())
		displayName = QCoreApplication::applicationName();
	return displayName;
}

QIcon
HelpMenu::iconFromAboutData(const KAboutData &aboutData)
{
	QIcon icon;
	const QVariant logo = aboutData.programLogo();
  
	switch (logo.typeId()) {
	case QMetaType::QIcon:
		icon = logo.value<QIcon>();
		break;
	case QMetaType::QPixmap:
		icon = QIcon(logo.value<QPixmap>());
		break;
	case QMetaType::QImage:
		icon = QIcon(QPixmap::fromImage(logo.value<QImage>()));
		break;
	default:
		break;
	}
 
	if (icon.isNull())
		icon = QGuiApplication::windowIcon();
 
	return icon;
}
