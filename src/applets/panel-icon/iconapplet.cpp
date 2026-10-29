/*
    SPDX-FileCopyrightText: 2016 Kai Uwe Broulik <kde@privat.broulik.de>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "iconapplet.h"
#include "popuptitle.h"

#include <QAction>
#include <QDir>
#include <QFileInfo>
#include <QIcon>

#include <KAuthorized>
#include <KDesktopFile>
#include <KFileUtils>
#include <KLocalizedString>
#include <KNotificationJobUiDelegate>
#include <KService>
#include <KServiceAction>
#include <KPropertiesDialog>
#include <KApplicationTrader>
#include <KProtocolManager>

#include <KIO/ApplicationLauncherJob>
#include <KIO/FavIconRequestJob>
#include <KIO/OpenFileManagerWindowJob>
#include <KIO/OpenUrlJob>
#include <KIO/StatJob>

IconApplet::IconApplet(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
    : Plasma::Applet(parent, data, args)
{
}

IconApplet::~IconApplet()
{
    // in a handler connected to IconApplet::appletDeleted m_localPath will be empty?!
    if (destroyed()) {
        QFile::remove(m_localPath);
    }
}

void IconApplet::init()
{
    populate();
}

void IconApplet::configChanged()
{
    populate();
}

void IconApplet::populate()
{
	const QUrl confUrl = url();

    // our backing desktop file already exists? just read all the things from it
    const QString path = localPath();
    if (QFileInfo::exists(path)) {
        populateFromDesktopFile(path);
        return;
    }

    if (!confUrl.isValid()) {
        // invalid confUrl, use dummy data
        populateFromDesktopFile(QString());
        return;
    }

    const QString plasmaIconsFolderPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/plasma_icons");
    if (!QDir().mkpath(plasmaIconsFolderPath)) {
        setLaunchErrorMessage(i18n("Failed to create icon widgets folder '%1'", plasmaIconsFolderPath));
        return;
    }

    setBusy(true); // unset in populateFromDesktopFile where we'll end up in if all goes well

    auto *statJob = KIO::stat(confUrl, KIO::HideProgressInfo);
    connect(statJob, &KIO::StatJob::finished, this, [=, this] {
        QString desiredDesktopFileName = confUrl.fileName();

        // in doubt, just encode the entire URL, e.g. https://www.kde.org/ has no filename
        if (desiredDesktopFileName.isEmpty()) {
            desiredDesktopFileName = KIO::encodeFileName(confUrl.toDisplayString());
        }

        // We always want it to be a .desktop file (e.g. also for the "Type=Link" at the end)
        if (!desiredDesktopFileName.endsWith(QLatin1String(".desktop"))) {
            desiredDesktopFileName.append(QLatin1String(".desktop"));
        }

        QString backingDesktopFile = plasmaIconsFolderPath + QLatin1Char('/');
        // KFileUtils::suggestName always appends a suffix, i.e. it expects that we already know the file already exists
        if (QFileInfo::exists(backingDesktopFile + desiredDesktopFileName)) {
            desiredDesktopFileName = KFileUtils::suggestName(QUrl::fromLocalFile(plasmaIconsFolderPath), desiredDesktopFileName);
        }
        backingDesktopFile.append(desiredDesktopFileName);

        QString name; // ends up as "Name" in the .desktop file for "Link" files below

        const QUrl statJobUrl = statJob->mostLocalUrl();
        if (statJobUrl.isLocalFile()) {
            const QString localUrlString = statJobUrl.toLocalFile();

            // if desktop file just copy it over
            if (KDesktopFile::isDesktopFile(localUrlString)) {
                // if this restriction is set, KIO won't allow running desktop files from outside
                // registered services, applications, and so on, in this case we'll use the original
                // .desktop file and lose the ability to customize it
                if (!KAuthorized::authorize(KAuthorized::RUN_DESKTOP_FILES)) {
                    populateFromDesktopFile(localUrlString);
                    // we don't call setLocalPath here as we don't want to store localPath to be a system-location
                    // so that the fact that we cannot edit is re-evaluated every time
                    return;
                }

                if (!QFile::copy(localUrlString, backingDesktopFile)) {
                    setLaunchErrorMessage(i18n("Failed to copy icon widget desktop file from '%1' to '%2'", localUrlString, backingDesktopFile));
                    setBusy(false);
                    return;
                }

                // set executable flag on the desktop file so KIO doesn't complain about executing it
				QFile::setPermissions(backingDesktopFile, QFile::permissions(backingDesktopFile) | QFile::ExeOwner);

                populateFromDesktopFile(backingDesktopFile);

                return;
            }
        }

        // in all other cases just make it a link

        QString iconName;
        QString genericName;

        if (!statJob->error()) {
            KFileItem item(statJob->statResult(), statJobUrl);
			name = item.text();

            if (item.mimetype() != QLatin1String("application/octet-stream")) {
                iconName = item.iconName();
                genericName = item.mimeComment();
            }
        }

        // KFileItem might return "." as text for e.g. root folders
        if (name == QLatin1Char('.')) {
            name.clear();
        }

        if (name.isEmpty()) {
            name = statJobUrl.fileName();
        }

        if (name.isEmpty()) {
            // TODO would be cool to just show the parent folder name instead of the full path
            name = statJobUrl.path();
        }

        // For websites the filename e.g. "index.php" is usually not what you want
        // also "/" isn't very descript when it's not our local "root" folder
        if (name.isEmpty() || statJobUrl.scheme().startsWith(QLatin1String("http")) || (!statJobUrl.isLocalFile() && name == QLatin1String("/"))) {
            name = statJobUrl.host();
        }

        if (iconName.isEmpty()) {
            // In doubt ask KIO::iconNameForUrl, KFileItem can't cope with http:// URLs for instance
            iconName = KIO::iconNameForUrl(statJobUrl);
        }

        bool downloadFavIcon = false;

        if (statJobUrl.scheme().startsWith(QLatin1String("http"))) {
            const QString favIcon = KIO::favIconForUrl(statJobUrl);

            if (!favIcon.isEmpty()) {
                iconName = favIcon;
            } else {
                downloadFavIcon = true;
            }
        }

        KDesktopFile linkDesktopFile(backingDesktopFile);
        auto desktopGroup = linkDesktopFile.desktopGroup();

        desktopGroup.writeEntry(QStringLiteral("Name"), name);
        desktopGroup.writeEntry(QStringLiteral("Type"), QStringLiteral("Link"));
        desktopGroup.writeEntry(QStringLiteral("URL"), statJobUrl);
        desktopGroup.writeEntry(QStringLiteral("Icon"), iconName);
        if (!genericName.isEmpty()) {
            desktopGroup.writeEntry(QStringLiteral("GenericName"), genericName);
        }

        linkDesktopFile.sync();

        populateFromDesktopFile(backingDesktopFile);

        if (downloadFavIcon) {
            auto *job = new KIO::FavIconRequestJob(confUrl);
            connect(job, &KIO::FavIconRequestJob::result, this, [job, backingDesktopFile, this](KJob *) {
                if (!job->error()) {
                    KDesktopFile(backingDesktopFile).desktopGroup().writeEntry(QStringLiteral("Icon"), job->iconFile());

                    setIcon(job->iconFile());
                }
            });
        }
    });
}


void IconApplet::populateFromDesktopFile(const QString &path)
{
    // path empty? just set icon to "unknown" and call it a day
    if (path.isEmpty()) {
        setIcon(QStringLiteral("unknown"));
        return;
    }

    KDesktopFile desktopFile(path);

    const QString &name = desktopFile.readName();
	if (title() != name)
		setTitle(name);

    const QString &genericName = desktopFile.readGenericName();
    if (m_genericName != genericName) {
        m_genericName = genericName;
        Q_EMIT genericNameChanged(genericName);
    }

	const QString &icon = desktopFile.readIcon();
    setIcon(!icon.isEmpty() ? icon : QStringLiteral("unknown"));

    setLocalPath(path);

    setBusy(false);
}

QUrl IconApplet::url() const
{
    return config().readEntry(QStringLiteral("url"), QUrl());
}

void IconApplet::setUrl(const QUrl &newUrl)
{
    if (url() != newUrl) {
		config().writeEntry(QStringLiteral("url"), newUrl);
        Q_EMIT urlChanged(newUrl);       
        populate();
    }
}

QString IconApplet::genericName() const
{
    return m_genericName;
}

bool IconApplet::isValid() const
{
    return !m_localPath.isEmpty();
}

QList<QAction *>
IconApplet::contextualActions()
{
	// First clear out the old set of actions
	for (auto action : m_actions)
		action->deleteLater();
	m_actions.clear();

    if (m_localPath.isEmpty()) {
        return Applet::contextualActions();
	}
	
    KDesktopFile desktopFile(m_localPath);	
	if (desktopFile.hasLinkType()) {
		// Link type means the icon points to a URL, so add URL specific actions
        const QUrl linkUrl = QUrl(desktopFile.readUrl());
        if (linkUrl.isValid() && !linkUrl.scheme().isEmpty()) {
			const KFileItem fileItem(linkUrl);
			const KService::List services = KApplicationTrader::queryByMimeType(fileItem.mimetype());
			for (const KService::Ptr &service : services) {
				if (service->noDisplay())
					continue;

				auto *action = new QAction(QIcon::fromTheme(service->icon()), service->name(), this);
				connect(action, &QAction::triggered, this, [linkUrl, service]() {
					auto *job = new KIO::ApplicationLauncherJob(service);
					job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoErrorHandlingEnabled));
					job->setUrls({linkUrl});
					job->start();
				});
				m_actions.append(action);
			}					
			if (!m_actions.isEmpty())
				m_actions.prepend(new PopupTitle(QIcon::fromTheme(QStringLiteral("document-open")),
												 i18n("Open With"), this));

			if (KProtocolManager::supportsListing(linkUrl)) {
				// Now we add the open in file explorer action.
				if (!m_actions.isEmpty()) {
					auto *separator = new QAction(this);
					separator->setSeparator(true);
					m_actions.append(separator);
				}
				auto *explorerAction = new QAction(QIcon::fromTheme(QStringLiteral("system-file-manager")),
												   i18n("Open in File Explorer"), this);
				connect(explorerAction, &QAction::triggered, this, [linkUrl]() {
					auto *job = new KIO::OpenFileManagerWindowJob();
					job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoErrorHandlingEnabled));
					job->setHighlightUrls({linkUrl});
					job->start();
				});
				m_actions.append(explorerAction);
			}
		}
	} else {
		// Otherwise, we just consider the icon to be a service and query the serviceactions
		const KService service(m_localPath);

		const auto jumpListActions = service.actions();
		for (const KServiceAction &serviceAction : jumpListActions) {
			if (serviceAction.noDisplay())
				continue;

			auto *action = new QAction(QIcon::fromTheme(serviceAction.icon()), serviceAction.text(), this);
			if (serviceAction.isSeparator())
				action->setSeparator(true);

			connect(action, &QAction::triggered, this, [serviceAction]() {
				auto *job = new KIO::ApplicationLauncherJob(serviceAction);
				job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoErrorHandlingEnabled));
				job->start();
			});

		    m_actions.append(action);
		}

		if (!m_actions.isEmpty())
			m_actions.prepend(new PopupTitle(QIcon::fromTheme(service.icon()), service.name(), this));
	}

	// Append a separator to separate out contextual actions from the internal actions
	if (!m_actions.isEmpty()) {
		auto *separator = new QAction(this);
		separator->setSeparator(true);
		m_actions.append(separator);
	}
	
	return m_actions + Applet::contextualActions();
}

void IconApplet::run()
{
	if (m_localPath.isEmpty())
		return;
    auto *job = new KIO::OpenUrlJob(QUrl::fromLocalFile(m_localPath));
    job->setRunExecutables(true); // so it can launch apps
    job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
    job->start();
}

void IconApplet::configure()
{
	if (m_localPath.isEmpty())
		return;
	
    KPropertiesDialog *dialog = m_configDialog.data();
    if (dialog) {
        dialog->show();
        dialog->raise();
        return;
    }

    dialog = new KPropertiesDialog(QUrl::fromLocalFile(m_localPath));
    m_configDialog = dialog;

    connect(dialog, &KPropertiesDialog::applied, this, [this] {
        KDesktopFile desktopFile(m_localPath);
        if (desktopFile.hasLinkType()) {
            const QUrl newUrl(desktopFile.readUrl());

            if (url() != newUrl) {
                // make sure to fully repopulate in case the user changed the Link URL
                QFile::remove(m_localPath);

                setUrl(newUrl); // calls populate() itself, but only if it changed
                return;
            }
        }

        populate();
    });

    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    dialog->setFileNameReadOnly(true);
    dialog->setWindowIcon(QIcon::fromTheme(QStringLiteral("document-properties")));
    dialog->show();
}

QString IconApplet::localPath() const
{
    return config().readEntry(QStringLiteral("localPath"));
}

void IconApplet::setLocalPath(const QString &localPath)
{
    const bool oldValid = isValid();
    m_localPath = localPath;
    config().writeEntry(QStringLiteral("localPath"), localPath);
    if (oldValid != isValid()) {
        Q_EMIT isValidChanged();
    }
}

K_PLUGIN_CLASS_WITH_JSON(IconApplet, "metadata.json")

#include "iconapplet.moc"
