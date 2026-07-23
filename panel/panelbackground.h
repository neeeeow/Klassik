/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "../common/klassikqstyleitem.h"

#include <QPainter>
#include <QImage>
#include <Plasma/Plasma>

class PanelBackground : public KlassikQStyleItem
{
	/*
	  This class draws a sunken KDE 3 style panel background
	*/
	  
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(bool useBackground READ useBackground WRITE setUseBackground)
	Q_PROPERTY(bool colorizeBackground READ colorizeBackground WRITE setColorizeBackground)
	Q_PROPERTY(Plasma::Types::Location panelLocation READ panelLocation WRITE setPanelLocation)
public:
	PanelBackground(QQuickItem *parent = nullptr);
	~PanelBackground() override = default;

	bool useBackground() const { return m_useBackground; }
	void setUseBackground(bool state) {
		if (m_useBackground != state) {
			m_useBackground = state;
			updateImage();
		}
	}

	bool colorizeBackground() const { return m_colorizeBackground; }
	void setColorizeBackground(bool state) {
		if (m_colorizeBackground != state) {
			m_colorizeBackground = state;
			loadBackground();
			updateImage();
		}
	}

    Plasma::Types::Location panelLocation() const { return m_panelLocation; }
	void setPanelLocation(Plasma::Types::Location location) {
		if (m_panelLocation != location) {
			m_panelLocation = location;
			updateImage();
		}
	}

protected:
	void componentComplete() override;
	void paint(QPainter *p) const override;

private:
	bool m_useBackground = true;
	bool m_colorizeBackground = false;
	Plasma::Types::Location m_panelLocation = Plasma::Types::BottomEdge;
	QImage m_background;

	bool event(QEvent *event) override;
	
	void loadBackground();
	void colorize(QImage &image);
};
