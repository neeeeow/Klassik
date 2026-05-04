#pragma once

#include <QStyle>
#include <QWidgetAction>
#include <QLabel>
#include <QFont>
#include <QPainter>
#include <QFontMetrics>
#include <QStyleOptionMenuItem>

class PopupMenuTitle : public QWidgetAction
{

public:
	PopupMenuTitle(const QString &title, QWidget *parent = nullptr)
		: QWidgetAction(parent), m_text(title)

	{
		setEnabled(false); // by default, make it disabled
	}

protected:
	QWidget* createWidget(QWidget *parent) override
	{
		return new TitleWidget(m_text, parent); // Create our header inside of the title
	}

private:
	QString m_text; // The text itself

	// The header widget that we'll draw within the QWidgetAction
	class TitleWidget : public QWidget
	{
	public:
		TitleWidget(const QString &text, QWidget *parent)
			: QWidget(parent), m_text(text)
		{
			m_font = font();
			m_font.setBold(true);
			setEnabled(false);
		}

	protected:
		void paintEvent(QPaintEvent *) override
		{
			// This is a straight port from KDE 3's K Menu
			QPainter p(this);
			QRect r = rect();

			// Draw the background
			QStyleOptionHeader opt;
			opt.initFrom(this);
			opt.rect = r; // NOTE: PE_HeaderSection is replaced by CE_HeaderSection
			style()->drawControl(QStyle::CE_HeaderSection, &opt, &p, this);

			// Draw the text
			if (!m_text.isEmpty()) {
				p.setPen(palette().buttonText().color());
				p.setFont(m_font);
				p.drawText(r, Qt::AlignCenter | Qt::TextSingleLine, m_text);
			}

			// Top highlight line (not sure what purpose this serves...)
			p.setPen(palette().highlight().color());
			p.drawLine(0, 0, r.right(), 0);
		}
		
		QSize sizeHint() const override
		{
			QFontMetrics fm(m_font);
			QSize size = fm.size(Qt::TextSingleLine, m_text);
			size.setHeight(fm.height() + (style()->pixelMetric(QStyle::PM_DefaultFrameWidth) * 2) + 2);
			return size;
		}

	private:
		QString m_text;
		QFont m_font;
	};
};

