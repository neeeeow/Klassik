/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "iconapplet.h"
#include "dirmenu.h"
#include "servicegroupmenu.h"

#include <KPluginFactory>
#include <KLocalizedString>
#include <KProtocolInfo>

#include <KIO/StatJob>

using namespace Qt::StringLiterals;

IconApplet::IconApplet(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
	: ServiceMenuApplet(parent, data, args)
{
	setHasConfigurationInterface(false);
}

IconApplet::~IconApplet()
{
	if (m_statJob)
		m_statJob->kill(KJob::Quietly);
}

void
IconApplet::init()
{
	ServiceMenuApplet::init();
		
	connect(this, &IconApplet::urlChanged, this, qOverload<const QUrl &>(&ServiceMenuApplet::setInfo));
	connect(this, &IconApplet::urlChanged, this, &IconApplet::resolveUrl);
	connect(this, &ServiceMenuApplet::infoChanged, this, &IconApplet::populate);

	Q_EMIT urlChanged(config().readEntry(QStringLiteral("url"), QUrl()));
}

void
IconApplet::configChanged()
{
	ServiceMenuApplet::configChanged();
	const QUrl newUrl = config().readEntry(QStringLiteral("url"), QUrl());
	if (info().url() != newUrl)
		Q_EMIT urlChanged(newUrl);
}

ServiceMenu *
IconApplet::createMenu()
{
	switch (info().type()) {
	case ItemInfo::Type::ServiceGroup:
		return new ServiceGroupMenu(info(), this);
	case ItemInfo::Type::Directory:
		return new DirMenu(info(), this);
	default:
		return nullptr;
	}	
}

void
IconApplet::populate()
{
	if (!info().isValid()) {
		setTitle(i18n("Unknown"));
		setIcon(QStringLiteral("unknown"));
		return;
	}
	setTitle(info().name());
	setIcon(info().icon());
}

void
IconApplet::setUrl(const QUrl &url)
{
	if (info().url() == url)
		return;

	config().writeEntry(QStringLiteral("url"), url);
	Q_EMIT urlChanged(url);
	Q_EMIT configNeedsSaving();
}

void
IconApplet::resolveUrl(const QUrl &url)
{
	// Kill any existing statjobs before starting any new ones
	if (m_statJob)
		m_statJob->kill(KJob::Quietly);
	m_statJob.clear();

	// If the url is not a local file (and not an applications:/// group),
	// try to resolve it to a local file via a KIO::StatJob.
	if (url.isValid()
		&& !url.isLocalFile()
		&& url.scheme() != "applications"_L1
		&& KProtocolInfo::protocolClass(url.scheme()) == ":local"_L1) {
		m_statJob = KIO::mostLocalUrl(url, KIO::HideProgressInfo);
		connect(m_statJob, &KJob::result, this, &IconApplet::updateUrlFromJob);
	}
}

void
IconApplet::updateUrlFromJob(KJob *job)
{
	if (m_statJob == job)
		m_statJob.clear();
	
	if (!job || job->error())
		return;

	if (auto *statJob = qobject_cast<KIO::StatJob *>(job)) {
		const QUrl url = statJob->mostLocalUrl();
		if (url.isLocalFile())
			setUrl(url);
	}
}

void
IconApplet::run()
{
	if (hasMenu())
		toggleMenu();
	else {	
		if (!info().isValid())
			return;

		if (const KService::Ptr service = info().service())
			PanelMenu::execService(service);
		else
			PanelMenu::execUrl(info().url());
	}
}

K_PLUGIN_CLASS_WITH_JSON(IconApplet, "metadata.json")

#include "iconapplet.moc"
