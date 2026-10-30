/*
    SPDX-FileCopyrightText: 2016 Kai Uwe Broulik <kde@privat.broulik.de>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#pragma once

#include "servicemenuapplet.h"

#include <QPointer>

class KPropertiesDialog;

class IconApplet : public ServiceMenuApplet
{
    Q_OBJECT

	Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
	
    Q_PROPERTY(QString genericName READ genericName NOTIFY genericNameChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY isValidChanged)

public:
    explicit IconApplet(QObject *parent, const KPluginMetaData &data, const QVariantList &args);
    ~IconApplet() override;

    void init() override;
    void configChanged() override;

    QUrl url() const;
    void setUrl(const QUrl &newUrl);
	
    QString genericName() const;
    bool isValid() const;

    QList<QAction *> contextualActions() override;

    Q_INVOKABLE void run();
    Q_INVOKABLE void configure();

Q_SIGNALS:
	void urlChanged(const QUrl &url);
    void genericNameChanged(const QString &genericName);
    void isValidChanged();

private:
    void populate();
    void populateFromDesktopFile(const QString &path);

	ServiceMenu *createMenu() override;

    QString localPath() const;
    void setLocalPath(const QString &localPath);

    QString m_localPath;
    QString m_genericName;

	// QList containing the extra actions we add to the applet's contextual actions
	QList<QAction *> m_actions;

	// Pointer to the open file properties dialog. We keep a track of this variable in case
	// multiple properties windows are open
    QPointer<KPropertiesDialog> m_configDialog;
};
