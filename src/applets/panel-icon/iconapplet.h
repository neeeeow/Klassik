/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicemenuapplet.h"

class IconApplet final : public ServiceMenuApplet
{
	Q_OBJECT

	// The url the applet points to
	Q_PROPERTY(QUrl url READ appletUrl WRITE setUrl NOTIFY urlChanged)
	// Whether or not the url is valid
	Q_PROPERTY(bool valid READ isValid NOTIFY urlChanged)
	// The generic name of the applet
	Q_PROPERTY(QString genericName READ genericName NOTIFY genericNameChanged)	

public:
	IconApplet(QObject *parent, const KPluginMetaData &data, const QVariantList &args);

	QUrl appletUrl() const override;
	void setUrl(const QUrl &newUrl);
	bool isValid() const;
	QString genericName() const;	

	Q_INVOKABLE void run();

public Q_SLOTS:
	void init() override;

Q_SIGNALS:
	void urlChanged(const QUrl &url);
	void genericNameChanged(const QString &genericName);

private:
	ServiceMenu *createMenu() override;

	// Sets the generic name, and emits the required signal. Never called externally
	void setGenericName(const QString &genericName);	
	QString m_genericName;

private Q_SLOTS:
	// Populates the icon applet
	void populate(const QUrl &url);
};
