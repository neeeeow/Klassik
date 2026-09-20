/*
 *  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>
 *
 *  SPDX-FileCopyrightText: 2008-2009 Lukas Appelhans <l.appelhans@gmx.de>
 *  SPDX-FileCopyrightText: 2010-2011 Ingomar Wesp <ingomar@wesp.name>
 *  SPDX-FileCopyrightText: 2013 Bhushan Shah <bhush94@gmail.com>
 *  SPDX-FileCopyrightText: 2015 David Rosca <nowrep@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "quicklaunch_p.h"

#include <KIO/CommandLauncherJob>
#include <KIO/ApplicationLauncherJob>
#include <KNotificationJobUiDelegate>
#include <KOpenWithDialog>
#include <KService>

QuicklaunchPrivate::QuicklaunchPrivate(QObject *parent)
    : QObject(parent)
{
}

QVariantMap QuicklaunchPrivate::launcherData(const QString &storageId)
{
    QString name;
    QString icon;
    QString genericName;
    QVariantList jumpListActions;

	if (const KService::Ptr service = KService::serviceByStorageId(storageId)) {
		name = service->name();
        icon = service->icon();
        genericName = service->genericName();

		const QList<KServiceAction> actions = service->actions();
		for (const KServiceAction &action : actions) {
			const QString &name = action.text();
			const QString &exec = action.exec();
			if (name.isEmpty() || exec.isEmpty()) {
				continue;
			}

			jumpListActions << QVariantMap{{QStringLiteral("name"), name},
				{QStringLiteral("icon"), action.icon()},
				{QStringLiteral("exec"), exec}};
		}
	}

    return QVariantMap{{QStringLiteral("applicationName"), name},
                       {QStringLiteral("iconName"), icon},
                       {QStringLiteral("genericName"), genericName},
                       {QStringLiteral("jumpListActions"), jumpListActions}};
}

void QuicklaunchPrivate::openLauncher(const QString &storageId)
{
	const KService::Ptr service = KService::serviceByStorageId(storageId);
	if (!service)
		return;

	KIO::ApplicationLauncherJob *job = new KIO::ApplicationLauncherJob(service);
	job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
	job->start();
}

void QuicklaunchPrivate::openExec(const QString &exec)
{
    KIO::CommandLauncherJob *job = new KIO::CommandLauncherJob(exec);
    job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
    job->start();
}

void QuicklaunchPrivate::addLauncher()
{
    KOpenWithDialog *dialog = new KOpenWithDialog();
    dialog->setModal(false);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->hideRunInTerminal();
    dialog->setSaveNewApplications(true);
    dialog->show();

    connect(dialog, &KOpenWithDialog::accepted, this, [this, dialog]() {
        if (dialog->service())
			Q_EMIT launcherAdded(dialog->service()->storageId());
    });
}

QUrl
QuicklaunchPrivate::storageIdToUrl(const QString &storageId) const
{
	// Convert a storageId to an absolute QUrl
	const KService::Ptr service = KService::serviceByStorageId(storageId);
    if (!service)
        return QUrl();
    return QUrl::fromLocalFile(service->entryPath());
}

QString
QuicklaunchPrivate::urlToStorageId(const QUrl &url) const
{
	// Convert an absolute QUrl to a storageId
	const KService::Ptr service = KService::serviceByDesktopPath(url.toLocalFile());
	if (!service)
		return QString();
	return service->storageId();
}
