/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QWidgetAction>

class KMenuSearchAction : public QWidgetAction
{
	/* QWidgetAction which creates the lineedit used for the search functionality.
	   The lineedit text is given by the text() function */
	Q_OBJECT
public:
	explicit KMenuSearchAction(QObject *parent = nullptr);

Q_SIGNALS:
	void textChanged(const QString &text);

protected:
	QWidget *createWidget(QWidget *parent) override;
	bool eventFilter(QObject *obj, QEvent *event) override;
};
