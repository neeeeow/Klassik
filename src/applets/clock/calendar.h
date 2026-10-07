/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "panelpopupitem.h"

class QCalendarWidget;

class CalendarPopup : public PanelPopupItem
{
	Q_OBJECT
	QML_ELEMENT
public:
	explicit CalendarPopup(QQuickItem *parent = nullptr);

protected:
	void aboutToShow() override;

private:
	QCalendarWidget *m_calendar = nullptr;
	
    void initContents(QVBoxLayout *layout) override;
	void setCurrentDate();
};
