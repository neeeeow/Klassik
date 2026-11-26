/*
  SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicemenuapplet.h"

class KJob;

class IconApplet final : public ServiceMenuApplet
{
	Q_OBJECT

	// The url the applet points to
	Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)	

public:
	IconApplet(QObject *parent, const KPluginMetaData &data, const QVariantList &args);
	~IconApplet() override;

	QUrl url() const { return info().url(); }
	void setUrl(const QUrl &url);	

	Q_INVOKABLE void run();

public Q_SLOTS:
	void init() override;
	void configChanged() override;

Q_SIGNALS:
	void urlChanged(const QUrl &url);

private:
	ServiceMenu *createMenu() override;
	
	// The running statjob (for resolving non-local QUrls)
	QPointer<KJob> m_statJob;

private Q_SLOTS:
	// Populates the icon applet and sets the plasmoid name and icon
	void populate();

	// Resolves non-local Url, and sets it
	void resolveUrl(const QUrl &url);

	// Updates the QUrl from the result of a KJob
	void updateUrlFromJob(KJob *job);
};
