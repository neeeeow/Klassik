/*
    SPDX-FileCopyrightText: 2013-2016 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>
#include <QRect>

#include <qqmlregistration.h>

class QAction;
class QActionGroup;
class QQuickItem;

class Backend : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum MiddleClickAction {
        None = 0,
        Close,
        NewInstance,
        ToggleMinimized,
        ToggleGrouping,
        BringToCurrentDesktop,
    };

    Q_ENUM(MiddleClickAction)

    explicit Backend(QObject *parent = nullptr);
    ~Backend() override;

    Q_INVOKABLE void setActionGroup(QAction *action) const;

    Q_INVOKABLE QRect globalRect(QQuickItem *item) const;

private:
    QActionGroup *m_actionGroup = nullptr;
};
