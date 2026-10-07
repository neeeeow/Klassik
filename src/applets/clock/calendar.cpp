/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "calendar.h"

#include <QToolButton>
#include <QLineEdit>

CalendarPopup::CalendarPopup(QQuickItem *parent)
	: PanelPopupItem(parent)
{
}

void
CalendarPopup::initContents(QVBoxLayout *layout)
{
	// Initialise the QCalendarWidget
	if (m_calendar) {
		layout->removeWidget(m_calendar);
		m_calendar->deleteLater();
	}
	m_calendar = new QCalendarWidget();
	layout->addWidget(m_calendar);

	// Initialise the bottom layout
	auto bottomLayout = new QHBoxLayout();

	// Button which sets the calendar to today's date
	auto todayButton = new QToolButton();
	todayButton->setIcon(QIcon::fromTheme(QStringLiteral("go-jump-today")));
	connect(todayButton, &QToolButton::clicked, this, &CalendarPopup::setCurrentDate);
	bottomLayout->addWidget(todayButton);

	// Line edit displaying the date

	layout->addLayout(bottomLayout);

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
