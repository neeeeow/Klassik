/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "servicegroupmenuapplet.h"
#include "servicegroupmenu.h"

ServiceGroupMenuApplet::ServiceGroupMenuApplet(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args)
	: ServiceMenuApplet(parentObject, data, args) {}

void
ServiceGroupMenuApplet::init()
{
	ServiceMenuApplet::init();
	updateTitleAndIcon(relPath());
	connect(this, &ServiceGroupMenuApplet::relPathChanged, this, &ServiceGroupMenuApplet::initMenu, Qt::UniqueConnection);
	connect(this, &ServiceGroupMenuApplet::relPathChanged, this, &ServiceGroupMenuApplet::updateTitleAndIcon, Qt::UniqueConnection);
}

ServiceMenu *
ServiceGroupMenuApplet::createMenu()
{
	const KServiceGroup::Ptr group = KServiceGroup::group(relPath());
	if (!group->isValid())
		return nullptr;
	return new ServiceGroupMenu(group, this);
}

QString
ServiceGroupMenuApplet::relPath() const
{
	return config().readEntry(QStringLiteral("relPath"), QString());
}

void
ServiceGroupMenuApplet::setRelPath(const QString &newRelPath)
{
	if (relPath() != newRelPath) {
		config().writeEntry(QStringLiteral("relPath"), newRelPath);
		Q_EMIT relPathChanged(newRelPath);		 
	}
}

void
ServiceGroupMenuApplet::updateTitleAndIcon(const QString &relPath)
{
	const KServiceGroup::Ptr group = KServiceGroup::group(relPath);
	if (group->isValid()) {
		setTitle(group->caption());
		const QString groupIcon = group->icon();		
		setIcon(!groupIcon.isNull() ? groupIcon : QStringLiteral("applications-other"));
	} else {
	    setTitle(QString());
		setIcon(QStringLiteral("unknown"));
	}
}

K_PLUGIN_CLASS_WITH_JSON(ServiceGroupMenuApplet, "metadata.json")

#include "servicegroupmenuapplet.moc"
