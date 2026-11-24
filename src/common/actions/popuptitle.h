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
	    PopupTitleWidget(const QIcon &icon, const QString &title, QWidget *parent = nullptr);

		QSize sizeHint() const override;

	protected:
		void paintEvent(QPaintEvent *) override;
	
	private:
		QIcon m_icon;
		QString m_title;		
	};

}

class PopupTitle : public QWidgetAction
{
	/* Recreation of KDE 3.5's KPopupTitle */
	Q_OBJECT
public:
    explicit PopupTitle(const QString &title, QObject *parent = nullptr)
		: PopupTitle(QIcon(), title, parent) {}
	explicit PopupTitle(const QIcon &icon, const QString &title, QObject *parent = nullptr)
		: QWidgetAction(parent) {
		setIcon(icon);
		setText(title);		
	}

protected:
	QWidget* createWidget(QWidget *parent) override {
		return new Widgets::PopupTitleWidget(icon(), text(), parent);
	}
};
