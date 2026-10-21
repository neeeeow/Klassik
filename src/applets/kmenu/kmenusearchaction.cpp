/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "kmenusearchaction.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLineEdit>
#include <QStyle>
#include <QShortcut>
#include <QMenu>
#include <QEvent>
#include <QMouseEvent>
#include <QCoreApplication>

#include <KLocalizedString>

KMenuSearchAction::KMenuSearchAction(QObject *parent)
	: QWidgetAction(parent)
{
}

QWidget *
KMenuSearchAction::createWidget(QWidget *parent)
{
	auto *container = new QWidget(parent);
	container->setMouseTracking(true);

	// Layout which holds the button/lineedit
	auto *layout = new QHBoxLayout(container);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	// Tool button
	auto *clearBtn = new QToolButton(container);
	const int iconSize = container->style()->pixelMetric(QStyle::PM_SmallIconSize);
	clearBtn->setIconSize(QSize(iconSize, iconSize));
	clearBtn->setIcon(QIcon::fromTheme(QStringLiteral("edit-clear")));
	clearBtn->setAutoRaise(true);
	clearBtn->setMouseTracking(true);

	// Line edit
	auto *lineEdit = new QLineEdit(container);
	lineEdit->setPlaceholderText(i18n("Press '/' to search…"));
	lineEdit->setMouseTracking(true);

	// Connect signals
	connect(lineEdit, &QLineEdit::textChanged, this, &KMenuSearchAction::textChanged);
	connect(clearBtn, &QToolButton::clicked, lineEdit, &QLineEdit::clear);
	auto *shortcut = new QShortcut(Qt::Key_Slash, lineEdit);
	connect(shortcut, &QShortcut::activated, lineEdit, qOverload<>(&QLineEdit::setFocus));
	
	// Add the widgets to the layout
	layout->addWidget(clearBtn);
	layout->addWidget(lineEdit);

	// Install event filters
	container->installEventFilter(this);
	clearBtn->installEventFilter(this);
	lineEdit->installEventFilter(this);
	
	return container;
}

bool
KMenuSearchAction::eventFilter(QObject *obj, QEvent *event)
{
	auto *widget = qobject_cast<QWidget *>(obj);
	if (!widget)
		return QWidgetAction::eventFilter(obj, event);

	switch (event->type()) {
	case QEvent::MouseMove: {
		// We must propagate the mouse event to the menu!
		if (auto *menu = qobject_cast<QMenu *>(widget->window())) {
			auto *me = static_cast<QMouseEvent *>(event);
			const QPointF menuPos = widget->mapTo(menu, me->position());
			QMouseEvent forwarded(me->type(), menuPos, me->globalPosition(),
								  me->button(), me->buttons(), me->modifiers());
			QCoreApplication::sendEvent(menu, &forwarded);
		}
		break;
	}
	case QEvent::Enter:
		if (auto *lineEdit = qobject_cast<QLineEdit *>(obj))
			lineEdit->setFocus(Qt::MouseFocusReason);
		break;
	case QEvent::Leave:
		if (auto *lineEdit = qobject_cast<QLineEdit *>(obj))
			lineEdit->clearFocus();
		break;
	default:
		break;
	}
	return QWidgetAction::eventFilter(obj, event);
}
