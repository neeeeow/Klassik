/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QStyle>
#include <QWidgetAction>
#include <QFont>
#include <QPainter>
#include <QFontMetrics>
#include <QStyleOptionHeader>

// The header widget that we'll draw within the QWidgetAction
namespace KMenuWidgets {
	class TitleWidget : public QWidget
	{
		Q_OBJECT
	public:
		TitleWidget(const QString &text, QWidget *parent)
			: QWidget(parent), m_text(text) {
			setEnabled(false);
		}

		QSize sizeHint() const override {
			QFont f(font());
			f.setBold(true);
			const QFontMetrics fm(f);
			const int fw = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
			const QSize size = fm.size(Qt::TextSingleLine, m_text) + QSize(2*fw + 2, 2*fw + 2);
			return size;
		}

	protected:
		void paintEvent(QPaintEvent *) override {
			// This is a straight port from KDE 3's K Menu
			QPainter p(this);

			// Draw the background
			QStyleOptionHeader header;
			header.initFrom(this);
			style()->drawControl(QStyle::CE_HeaderSection, &header, &p, this);

			// Draw the text
			if (!m_text.isEmpty()) {
				QFont f(font());
				f.setBold(true);
				p.setPen(palette().buttonText().color());
				p.setFont(f);
				p.drawText(rect(), Qt::AlignCenter | Qt::TextSingleLine, m_text);
			}
		}	   

	private:
		QString m_text;
	};
}
	
class KMenuTitle : public QWidgetAction
{
	Q_OBJECT
public:
	KMenuTitle(const QString &title, QWidget *parent = nullptr)
		: QWidgetAction(parent) {
		setText(title);
		setEnabled(false); // by default, make it disabled
	}

protected:
	QWidget* createWidget(QWidget *parent) override {
		return new KMenuWidgets::TitleWidget(text(), parent); // Create our header inside of the title
	}
};

