/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QWidgetAction>
#include <QWidget>

namespace Widgets {
	// Wrap things up in the widgets namespace to avoid cluttering things
	class PopupTitleWidget : public QWidget
	{
		Q_OBJECT
	public:
	    PopupTitleWidget(const QString &title, const QIcon &icon, QWidget *parent = nullptr);

		QSize sizeHint() const override;

	protected:
		void paintEvent(QPaintEvent *) override;
	
	private:
		QString m_title;
		QIcon m_icon;
	};

}

class PopupTitle : public QWidgetAction
{
	/* Recreation of KDE 3.5's KPopupTitle */
	Q_OBJECT
public:
    explicit PopupTitle(const QString &title, QObject *parent = nullptr)
		: PopupTitle(title, QIcon(), parent) {}
	explicit PopupTitle(const QString &title, const QIcon &icon, QObject *parent = nullptr)
		: QWidgetAction(parent) {
		setText(title);
		setIcon(icon);
	}

protected:
	QWidget* createWidget(QWidget *parent) override {
		return new Widgets::PopupTitleWidget(text(), icon(), parent);
	}
};
