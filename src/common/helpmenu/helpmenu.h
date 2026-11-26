/*
  SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QMenu>
#include <QPointer>
#include <KAboutData>

class AboutDialog;

class KPluginMetaData;
class KAboutApplicationDialog;
class KBugReport;

class HelpMenu final : public QMenu
{
	/*
	  Custom Help menu to be used with Klassik applets and applications
	*/
	Q_OBJECT
public:
	explicit HelpMenu(QWidget *parent = nullptr);
    explicit HelpMenu(const KAboutData &aboutData, QWidget *parent = nullptr);
	explicit HelpMenu(const KPluginMetaData &plugin, QWidget *parent = nullptr);

public Q_SLOTS:
	// Invokes the about application dialog
	void aboutApplication();
	
	// Invokes the about Klassik dialog
	void aboutKlassik();

	// Opens the standard Report Bugs dialog box
	void reportBug();
	
private:
	// Converts a KPluginMetaData to KAboutData
	static KAboutData aboutDataFromMetaData(const KPluginMetaData &plugin);

	// Extracts a valid display name from a given KAboutData. If the name
	// is empty, it falls back to QGuiApplication/QCoreApplication
	static QString displayNameFromAboutData(const KAboutData &aboutData);

	// Extracts a QIcon from a given KAboutData
	static QIcon iconFromAboutData(const KAboutData &aboutData);

	// KAboutData
	KAboutData m_aboutData;

	// About Appliaction dialog
	QPointer<KAboutApplicationDialog> m_aboutApplication;

	// About Klassik dialog
	QPointer<AboutDialog> m_aboutDialog;

	// Report bug dialog
	QPointer<KBugReport> m_bugReport;
};
