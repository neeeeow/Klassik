/*
  SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QDialog>

class AboutDialog final : public QDialog
{
	/* About dialogue for Klassik */
	Q_OBJECT
public:
	explicit AboutDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

private:
	// Returns a QTextBrowser (wrapped in a QWidget with the correct margin) for the
	// about tab widget
	QWidget *addTextPage(const QString &html);
};
