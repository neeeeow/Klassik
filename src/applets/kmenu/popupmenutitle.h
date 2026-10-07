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
#include <QStyleOptionMenuItem>

// The header widget that we'll draw within the QWidgetAction
namespace menuWidgets {
	class TitleWidget : public QWidget
	{
		Q_OBJECT
	public:
		TitleWidget(const QString &text, QWidget *parent)
			: QWidget(parent), m_text(text) {
			m_font = font();
			m_font.setBold(true);
			setEnabled(false);
		}

	protected:
		void paintEvent(QPaintEvent *) override {
			// This is a straight port from KDE 3's K Menu
			QPainter p(this);

			// Draw the background
			QStyleOptionHeader opt;
			opt.initFrom(this);
			opt.rect = rect(); // NOTE: PE_HeaderSection is replaced by CE_HeaderSection
			style()->drawControl(QStyle::CE_HeaderSection, &opt, &p, this);

			// Draw the text
			if (!m_text.isEmpty()) {
				p.setPen(palette().buttonText().color());
				p.setFont(m_font);
				p.drawText(rect(), Qt::AlignCenter | Qt::TextSingleLine, m_text);
			}
		}
		
		QSize sizeHint() const override {
			const QFontMetrics fm(m_font);
			QSize size = fm.size(Qt::TextSingleLine, m_text);
			size.setHeight(fm.height() + (style()->pixelMetric(QStyle::PM_DefaultFrameWidth) * 2) + 2);
			return size;
		}

	private:
		QString m_text;
		QFont m_font;
	};
}
	
class PopupMenuTitle : public QWidgetAction
{
	Q_OBJECT
public:
	PopupMenuTitle(const QString &title, QWidget *parent = nullptr)
		: QWidgetAction(parent), m_text(title)

	{
		setEnabled(false); // by default, make it disabled
	}

protected:
	QWidget* createWidget(QWidget *parent) override
	{
		return new menuWidgets::TitleWidget(m_text, parent); // Create our header inside of the title
	}

private:
	QString m_text; // The text itself
};

