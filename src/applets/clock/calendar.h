/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "panelpopupitem.h"

#include <QCalendarWidget>

class QComboBox;

class CalendarPopup : public PanelPopupItem
{
	Q_OBJECT
	QML_ELEMENT
public:
	CalendarPopup(QQuickItem *parent = nullptr);
	~CalendarPopup() override = default;

protected:
	void aboutToShow() override;

private:
	QCalendarWidget *m_calendar = nullptr;
	
    void initContents(QVBoxLayout *layout) override;
	void setCurrentDate();
};
