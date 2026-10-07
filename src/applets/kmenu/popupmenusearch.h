/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QStyle>
#include <QWidgetAction>
#include <QStyleOptionMenuItem>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QToolButton>
#include <QShortcut>

namespace menuWidgets {
	class SearchWidget : public QWidget
	{
		Q_OBJECT
	public:
		SearchWidget(const QString &placeholderText, const QKeySequence &key = QKeySequence(), QWidget *parent = nullptr)
			: QWidget(parent) {
			
			QHBoxLayout *layout = new QHBoxLayout(this); // layout to hold the button and lineedit
			layout->setContentsMargins(0, 0, 0, 0);
			layout->setSpacing(0);
	
			QToolButton *clearBtn = new QToolButton(this);
			const int iconSize = style()->pixelMetric(QStyle::PM_SmallIconSize);
			const int height = sizeHint().height();
			clearBtn->setFixedSize(height, height); // ensure the button is square	
			clearBtn->setIconSize(QSize(iconSize, iconSize));
			clearBtn->setIcon(QIcon::fromTheme(QStringLiteral("edit-clear")));
			clearBtn->setAutoRaise(true);
			clearBtn->setFocusPolicy(Qt::StrongFocus);
			clearBtn->setMouseTracking(true);
	
			m_searchLineEdit = new QLineEdit(this);
			m_searchLineEdit->setPlaceholderText(placeholderText);
			m_searchLineEdit->setFocusPolicy(Qt::StrongFocus);
			m_searchLineEdit->setMouseTracking(true);
			m_searchLineEdit->installEventFilter(this);

			if (!key.isEmpty()) {
				QShortcut *searchShortcut = new QShortcut(key, this);
				connect(searchShortcut, &QShortcut::activated, m_searchLineEdit, qOverload<>(&QLineEdit::setFocus));
			}		  						
			connect(clearBtn, &QToolButton::clicked, m_searchLineEdit, &QLineEdit::clear);

			layout->addWidget(clearBtn);
			layout->addWidget(m_searchLineEdit);
		}

		QLineEdit* lineEdit() const { return m_searchLineEdit; }

	protected:
		QSize sizeHint() const override {
			QSize size = QWidget::sizeHint();
			const int iconSize = style()->pixelMetric(QStyle::PM_SmallIconSize); // The size of menu item icons (by default)
			QStyleOptionMenuItem opt; // Call sizeFromContents to get the height of a menu item in the current QStyle,
			opt.initFrom(this);       // ensuring the search bar is the same height as a menu item, giving a more consistent look
			const int menuHeight = style()->sizeFromContents(QStyle::CT_MenuItem, &opt, QSize(0, qMax(opt.fontMetrics.height(), iconSize))).height();
			size.setHeight(menuHeight);
			return size;
		}

		bool eventFilter(QObject *object, QEvent *e) override {
			// Give the search bar focus as soon as the mouse enters it
			if (object == m_searchLineEdit) {
				if (e->type() == QEvent::Enter)
					m_searchLineEdit->setFocus(Qt::MouseFocusReason);
				else if (e->type() == QEvent::Leave)
					m_searchLineEdit->clearFocus();
			}
			return QWidget::eventFilter(object, e);
		}


	private:		
		QLineEdit *m_searchLineEdit = nullptr;		
	};
}

class PopupMenuSearch : public QWidgetAction
{
	Q_OBJECT
public:
	PopupMenuSearch(const QString &placeholderText, const QKeySequence &key = QKeySequence(), QWidget *parent = nullptr)
		: QWidgetAction(parent)

	{
		menuWidgets::SearchWidget *widget = new menuWidgets::SearchWidget(placeholderText, key, parent);
		setDefaultWidget(widget); // each PopupMenuSearch should only be created once, so avoid createWidget
	}

    QLineEdit* lineEdit() const {
		if (menuWidgets::SearchWidget *widget = qobject_cast<menuWidgets::SearchWidget *>(defaultWidget()))
			return widget->lineEdit();
		else
			return nullptr;
	}
};
