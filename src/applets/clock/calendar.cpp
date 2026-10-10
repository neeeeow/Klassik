/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "calendar.h"

#include <QCalendarWidget>
#include <QToolButton>
#include <QDate>
#include <QDateEdit>

#include <KLocalizedString>

CalendarPopup::CalendarPopup(QQuickItem *parent)
	: PanelPopupItem(parent)
{
}

void
CalendarPopup::initContents(QVBoxLayout *layout)
{
	clearLayout(layout);
	m_calendar = nullptr;
	
	// The layout *must* have a parent, otherwise all of our widgets will be orphans.
	QWidget *parent = layout->parentWidget();
	if (!parent)
		return;
	
	// Initialise the QCalendarWidget
	auto *calendar = new QCalendarWidget(parent);
	m_calendar = calendar;
	layout->addWidget(m_calendar);

	// Initialise the bottom layout
	auto *bottomLayout = new QHBoxLayout();

	// Button which sets the calendar to today's date
	auto *todayButton = new QToolButton(parent);
	todayButton->setIcon(QIcon::fromTheme(QStringLiteral("go-jump-today")));
	todayButton->setToolTip(i18n("Select the current day"));
	connect(todayButton, &QToolButton::clicked, this, &CalendarPopup::setCurrentDate);
	bottomLayout->addWidget(todayButton);

	// Date edit displaying the date
	auto *dateEdit = new QDateEdit(parent);
	dateEdit->setDate(m_calendar->selectedDate());
	bottomLayout->addWidget(dateEdit);

	// Connect signals between date edit and calendar
	connect(calendar, &QCalendarWidget::selectionChanged, dateEdit, [calendar, dateEdit]() {
		dateEdit->setDate(calendar->selectedDate());
	});
	connect(dateEdit, &QDateEdit::dateChanged, m_calendar, &QCalendarWidget::setSelectedDate);
	
	// Add the layout
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
