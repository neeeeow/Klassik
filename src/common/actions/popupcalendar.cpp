/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "popupcalendar.h"

#include <QMenu>
#include <KDatePicker>

PopupCalendar::PopupCalendar(QObject *parent)
	: QWidgetAction(parent) {}

QWidget *
PopupCalendar::createWidget(QWidget *parent)
{
	auto *datePicker = new KDatePicker(parent);
	if (auto *menu = qobject_cast<QMenu *>(parent)) {
		// Reset the date whenever the menu containing the action is opened
	    connect(menu, &QMenu::aboutToShow, datePicker, [datePicker]() {
            datePicker->setDate(QDate::currentDate());
        });
	}
	return datePicker;
}
