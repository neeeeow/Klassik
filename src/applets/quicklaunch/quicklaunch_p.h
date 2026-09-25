/*
 *  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>
 *
 *  SPDX-FileCopyrightText: 2015 David Rosca <nowrep@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#pragma once

#include <QObject>
#include <QUrl>
#include <QVariantMap>
#include <qqmlregistration.h>

class QuicklaunchPrivate : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Logic)

public:
    explicit QuicklaunchPrivate(QObject *parent = nullptr);

    Q_INVOKABLE QVariantMap launcherData(const QString &storageId);
	Q_INVOKABLE void openLauncher(const QString &storageId);
    Q_INVOKABLE void openExec(const QString &exec);

    Q_INVOKABLE void addLauncher();

	// Functions to convert between storageIDs and URLs
	// Internally, launchers are stored by the storageID, but
	// we need to convert to/from URLs for drag/drop functionality
	Q_INVOKABLE QUrl storageIdToUrl(const QString &storageId) const;
    Q_INVOKABLE QString urlToStorageId(const QUrl &url) const;

Q_SIGNALS:
    void launcherAdded(const QString &storageId);
};

