/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "calendar.h"

CalendarPopup::CalendarPopup(QQuickItem *parent)
	: PanelPopupItem(parent)
{
}

void
CalendarPopup::initContents(QVBoxLayout *layout)
{
	if (m_calendar) {
		m_calendar->close();
		m_calendar->deleteLater();
	}
	m_calendar = new QCalendarWidget(frame());
	layout->addWidget(m_calendar);
	setCurrentDate();
}

void
CalendarPopup::aboutToShow()
{
    setCurrentDate();
}

void
CalendarPopup::setCurrentDate()
{
	if (!m_calendar)
		return;
	m_calendar->setSelectedDate(QDate::currentDate());
}
