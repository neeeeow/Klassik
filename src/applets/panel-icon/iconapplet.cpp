/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "iconapplet.h"
#include "dirmenu.h"
#include "servicegroupmenu.h"

#include <KPluginFactory>
#include <KLocalizedString>
#include <KProtocolManager>
#include <KFileItem>

#include <KIO/StatJob>

using namespace Qt::StringLiterals;

IconApplet::IconApplet(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
	: ServiceMenuApplet(parent, data, args) {
	setHasConfigurationInterface(false);
}

void
IconApplet::init()
{
	ServiceMenuApplet::init();
		
	connect(this, &IconApplet::urlChanged, this, &IconApplet::populate);
	connect(this, &IconApplet::urlChanged, this, &IconApplet::initMenu);

	const QUrl url = appletUrl();
	populate(url);
	resolveUrl(url);
}

ServiceMenu *
IconApplet::createMenu()
{
	const QUrl url = appletUrl();	
	if (!url.isValid())
		return nullptr;

	if (const KServiceGroup::Ptr group = PanelMenu::serviceGroupFromUrl(url))
		return new ServiceGroupMenu(group, this);
	else if (url.isLocalFile() && KProtocolManager::supportsListing(url)) {
		const KFileItem file(url);
		if (file.isDir())
			return new DirMenu(file, this);
	}


	return nullptr;		
}

void
IconApplet::populate(const QUrl &url)
{
	QString name, icon, genericName;
	if (const KService::Ptr service = PanelMenu::serviceFromUrl(url)) {
		name = service->name();
		icon = !service->icon().isEmpty() ? service->icon() : QStringLiteral("application-x-executable");
		genericName = service->genericName();
	} else if (const KServiceGroup::Ptr group = PanelMenu::serviceGroupFromUrl(url)) {
		name = group->caption();
		icon = !group->icon().isEmpty() ? group->icon() : QStringLiteral("applications-other");
		genericName = i18n("Application Menu");
	} else if (url.isValid()) {
		const KFileItem item(url);
		name = item.text();
		if (name.isEmpty() || name == QLatin1Char('.'))
			name = url.fileName();
		if (name.isEmpty() || url.scheme().startsWith("http"_L1))
			name = url.host();
		if (name.isEmpty())
			name = url.path();
		icon = item.iconName();
		genericName = item.mimeComment();
	}

	setTitle(name);
	setIcon(!icon.isEmpty() ? icon : QStringLiteral("unknown"));
	setGenericName(genericName);
}

QUrl
IconApplet::appletUrl() const
{
	return config().readEntry(QStringLiteral("url"), QUrl());
}

void
IconApplet::setUrl(const QUrl &url)
{
	if (appletUrl() == url)
		return;

	config().writeEntry(QStringLiteral("url"), url);
	Q_EMIT urlChanged(url);
	Q_EMIT configNeedsSaving();

	// Resolve the url in case it is non-local
	resolveUrl(url);
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

QString
IconApplet::genericName() const
{
	return m_genericName;
}

void
IconApplet::setGenericName(const QString &genericName)
{
	if (genericName != m_genericName) {
		m_genericName = genericName;
		Q_EMIT genericNameChanged(genericName);
	}
}

bool
IconApplet::isValid() const
{
	return appletUrl().isValid();
}

void
IconApplet::run()
{
	if (hasMenu())
		toggleMenu();
	else {	
		const QUrl url = appletUrl();
		if (!url.isValid())
			return;

		if (const KService::Ptr service = PanelMenu::serviceFromUrl(url))
			PanelMenu::execService(service);
		else
			PanelMenu::execUrl(url);
	}
}

K_PLUGIN_CLASS_WITH_JSON(IconApplet, "metadata.json")

#include "iconapplet.moc"
