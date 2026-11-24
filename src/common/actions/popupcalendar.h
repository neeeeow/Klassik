/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QWidgetAction>

class PopupCalendar : public QWidgetAction
{
	/* QWidgetAction containing a KDatePicker - used for displaying dates */
	Q_OBJECT
public:
	explicit PopupCalendar(QObject *parent = nullptr);

protected:
	QWidget* createWidget(QWidget *parent) override;
};
