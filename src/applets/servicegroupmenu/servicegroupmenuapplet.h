/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "servicemenuapplet.h"

class ServiceGroupMenuApplet : public ServiceMenuApplet
{
	Q_OBJECT
	Q_PROPERTY(QString relPath READ relPath WRITE setRelPath NOTIFY relPathChanged)
	
public:
	ServiceGroupMenuApplet(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args);
	void init() override;

	// Returns the relpath used by the menu
	QString relPath() const;
	// Sets the relpath to be used
	void setRelPath(const QString &newRelPath);

Q_SIGNALS:
	void relPathChanged(const QString &relPath);

private:
	ServiceMenu* createMenu() override;

private Q_SLOTS:
	void updateTitleAndIcon(const QString &relPath);
};
