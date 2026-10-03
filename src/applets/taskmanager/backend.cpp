/*
    SPDX-FileCopyrightText: 2012-2016 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "backend.h"

#include <QAction>
#include <QActionGroup>
#include <QQuickItem>
#include <QQuickWindow>

Backend::Backend(QObject *parent)
    : QObject(parent)
    , m_actionGroup(new QActionGroup(this))
{
}

Backend::~Backend()
{
}

void Backend::setActionGroup(QAction *action) const
{
    if (action) {
        action->setActionGroup(m_actionGroup);
    }
}

QRect Backend::globalRect(QQuickItem *item) const
{
    if (!item || !item->window()) {
        return QRect();
    }

    QRect iconRect(item->x(), item->y(), item->width(), item->height());
    iconRect.moveTopLeft(item->parentItem()->mapToScene(iconRect.topLeft()).toPoint());
    iconRect.moveTopLeft(item->window()->mapToGlobal(iconRect.topLeft()));

    return iconRect;
}

#include "moc_backend.cpp"
