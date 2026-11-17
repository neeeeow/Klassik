/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "aboutdialog.h"

#include <QVBoxLayout>
#include <QLabel>

#include <KLocalizedString>

#define DIALOG_MARGIN 11
#define DIALOG_SPACING 6

AboutDialog::AboutDialog(QWidget *parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	setWindowTitle(i18n("About Klassik"));
	setWindowIcon(QIcon::fromTheme(QStringLiteral("help-about")));

	// Main dialog layout
	auto *dialogLayout = new QVBoxLayout(this);
	dialogLayout->setContentsMargins(DIALOG_MARGIN, DIALOG_MARGIN, DIALOG_MARGIN, DIALOG_MARGIN);
	dialogLayout->setSpacing(DIALOG_SPACING);

	// Title
	auto *title = new QLabel(this);
	title->setText(i18n("Klassik. Release %1", QStringLiteral(KLASSIK_VERSION)));
	dialogLayout->addWidget(title);
}
