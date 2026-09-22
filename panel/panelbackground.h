/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "../common/klassikqstyleitem.h"

#include <QPainter>
#include <QImage>
#include <QUrl>

#include <Plasma/Plasma>

class PanelBackground : public KlassikQStyleItem
{
	/*
	  This class draws a KDE 3 style panel background
	*/
	  
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(bool drawFrame READ drawFrame WRITE setDrawFrame)
	Q_PROPERTY(bool colorizePanel READ colorizePanel WRITE setColorizePanel)
	Q_PROPERTY(bool useBackground READ useBackground WRITE setUseBackground)
	Q_PROPERTY(Plasma::Types::Location panelLocation READ panelLocation WRITE setPanelLocation)
	Q_PROPERTY(bool useCustomBackground READ useCustomBackground WRITE setUseCustomBackground)
	Q_PROPERTY(QUrl customBackgroundUrl READ customBackgroundUrl WRITE setCustomBackgroundUrl)
public:
	PanelBackground(QQuickItem *parent = nullptr);
	~PanelBackground() override = default;

	bool drawFrame() const { return m_drawFrame; }
	void setDrawFrame(bool state) {
		if (m_drawFrame != state) {
			m_drawFrame = state;
			updateImage();
		}
	}
	
	bool colorizePanel() const { return m_colorizePanel; }
	void setColorizePanel(bool state) {
		if (m_colorizePanel != state) {
			m_colorizePanel = state;
			loadBackground();
			updateImage();
		}
	}

	bool useBackground() const { return m_useBackground; }
	void setUseBackground(bool state) {
		if (m_useBackground != state) {
			m_useBackground = state;
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

	bool useCustomBackground() const { return m_useCustomBackground; }
	void setUseCustomBackground(bool state) {
		if (m_useCustomBackground != state) {
			m_useCustomBackground = state;
			loadBackground();
			updateImage();
		}
	}
	
	QUrl customBackgroundUrl() const { return m_customBackgroundUrl; }
	void setCustomBackgroundUrl(QUrl url) {
		if (m_customBackgroundUrl != url) {
			m_customBackgroundUrl = url;
			loadBackground();
			updateImage();
		}
	}	

protected:
	void componentComplete() override;
	bool event(QEvent *event) override;

private:
	bool m_drawFrame = true;
	bool m_colorizePanel = false;
	bool m_useBackground = false;
	Plasma::Types::Location m_panelLocation = Plasma::Types::BottomEdge;
	bool m_useCustomBackground = false;
	QUrl m_customBackgroundUrl;
	QImage m_background;	

	void paint(QPainter *p) const override;
	void loadBackground();
	void colorize(QImage &image) const;
	QColor tintColor(const QColor &baseColor) const;
};
