/*
  SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "aboutdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTabWidget>
#include <QTextBrowser>
#include <QDialogButtonBox>
#include <QPushButton>

#include <KLocalizedString>
#include <KNotificationJobUiDelegate>

#include <KIO/OpenUrlJob>

#define DIALOG_MARGIN 11
#define DIALOG_SPACING 6

#define NEEEEOW_URL "https://github.com/neeeeow"
#define KLASSIK_URL "https://github.com/neeeeow/Klassik"
#define ISSUES_URL "https://github.com/neeeeow/Klassik/issues"
#define DONATE_URL "https://github.com/sponsors/neeeeow"

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
	auto *title = new QLabel(i18n("Klassik. Release %1", QStringLiteral(KLASSIK_VERSION)), this);
	title->setAlignment(Qt::AlignCenter);
	QFont font = title->font();
	font.setBold(true);
	font.setPointSize(font.pointSize() + 2);
	title->setFont(font);	
	dialogLayout->addWidget(title);

	// Main contents area (image + tab widget)
	auto *contents = new QHBoxLayout;
	dialogLayout->addLayout(contents);

	// Konqi image
	auto *image = new QLabel(this);
	image->setAlignment(Qt::AlignCenter);
	image->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	image->setLineWidth(1);

	QPalette pal = image->palette();
	pal.setColor(QPalette::Window, Qt::white);
	image->setPalette(pal);
	image->setAutoFillBackground(true);

	image->setPixmap(QPixmap(QStringLiteral(":/klassik/helpmenu/about.png")));
	contents->addWidget(image);

	// Tab widget
	auto *tabs = new QTabWidget(this);

	tabs->addTab(addTextPage(i18n("<html>"
								  "<b>Klassik</b>, by <a href=\"%1\">neeeeow</a>, is a free "
								  "and open-source recreation of the classic KDE 3 desktop "
								  "experience for KDE Plasma 6. Built from the ground up using "
								  "Qt 6, Qt Quick, and KDE Frameworks 6, it includes full support for modern "
								  "technologies such as Wayland and fractional scaling.<br /><br />"
								  "Everyone is welcome to use and contribute to Klassik, including you. "
								  "Visit <a href=\"%2\">%2</a> to learn more and view the source code."
								  "</html>",
								  QStringLiteral(NEEEEOW_URL),
								  QStringLiteral(KLASSIK_URL)
								 )), i18n("&About"));

	tabs->addTab(addTextPage(i18n("<html>"
								  "Software can always be improved, and issues will naturally "
								  "arise in Klassik. However, you — the user — must report when "
								  "something does not work as expected or could be done better.<br /><br />"
								  "Issues can be reported by visiting <a href=\"%1\">%1</a> or by "
								  "selecting the \"Report Bug…\" dialog from the \"Help\" menu. "
								  "Suggestions for improvement are also welcome on the issue tracker, "
								  "provided you select the \"Feature\" category."
								  "</html>",
								  QStringLiteral(ISSUES_URL)
								 )), i18n("&Report Bugs or Wishes"));

	tabs->addTab(addTextPage(i18n("<html>"
								  "Contributors are always welcome, and every contribution is "
								  "deeply appreciated. You do not have to be a software developer "
								  "to contribute to Klassik. You can provide graphics, themes, "
								  "translations, sounds, or improved documentation. You decide! A great "
								  "place to start is checking out the <a href=\"%1\">open issues</a>."
								  "<br /><br />"
								  "The use of LLMs to assist with development is <b>permitted</b>, provided "
								  "you genuinely understand the code you submit and do not blindly "
								  "copy-paste generated code. However, LLM-generated art is "
								  "strictly <b>prohibited</b>."
								  "</html>",
								  QStringLiteral(ISSUES_URL)
								 )), i18n("&Join In"));

	tabs->addTab(addTextPage(i18n("<html>"
								  "Klassik is, and will always remain, free of charge; however, creating "
								  "and maintaining it requires a great deal of time and effort. "
								  "Financial donations go a long way towards supporting "
								  "Klassik's development and can be made through the "
								  "<a href=\"%1\">GitHub Sponsors page</a>.<br /><br />"
								  "If you are not in a position to donate, that is completely fine. "
								  "Simply using Klassik and spreading the word is equally appreciated!"
								  "</html>",
								  QStringLiteral(DONATE_URL)
								 )), i18n("&Support Klassik"));
	
	contents->addWidget(tabs);

	// Button box
	auto *buttons = new QDialogButtonBox(QDialogButtonBox::Help | QDialogButtonBox::Close, this);
	if (QPushButton *help = buttons->button(QDialogButtonBox::Help)) {
		help->setText(i18n("&Donate"));
		help->setIcon(QIcon::fromTheme(QStringLiteral("help-donate")));
	}
	if (QPushButton *close = buttons->button(QDialogButtonBox::Close))
		close->setDefault(true);

	connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
	connect(buttons, &QDialogButtonBox::helpRequested, buttons, [] {
		auto *job = new KIO::OpenUrlJob(QUrl(QStringLiteral(DONATE_URL)));
		job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoHandlingEnabled));
		job->start();
	});
	
	dialogLayout->addWidget(buttons);

	resize(sizeHint());
}

QWidget *
AboutDialog::addTextPage(const QString &html)
{
	auto *container = new QWidget(this);
	auto *layout = new QVBoxLayout(container);
	layout->setContentsMargins(DIALOG_MARGIN, DIALOG_MARGIN, DIALOG_MARGIN, DIALOG_MARGIN);

	auto *browser = new QTextBrowser(container);
	browser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	browser->setOpenExternalLinks(true);
	browser->setHtml(html);
	browser->setMinimumWidth(fontMetrics().averageCharWidth() * 60);
	browser->setMinimumHeight(fontMetrics().lineSpacing() * 10);
	
	layout->addWidget(browser);

	return container;
}
