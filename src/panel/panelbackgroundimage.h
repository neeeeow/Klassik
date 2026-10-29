/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "painteditem.h"

#include <Plasma/Plasma>

class PanelBackgroundImage : public PaintedItem
{
	/* Class which draws the tiled background for the panel */
	Q_OBJECT
	QML_ELEMENT
	// The QUrl which points to the image
	Q_PROPERTY(QUrl source MEMBER m_source NOTIFY propertyChanged REQUIRED)
	// Whether or not to colorize the image (does not need to be set in QML, defaults to false)
	Q_PROPERTY(bool colorize MEMBER m_colorize NOTIFY propertyChanged)
	// Location of the panel
	Q_PROPERTY(Plasma::Types::Location location MEMBER m_location NOTIFY propertyChanged REQUIRED)
public:
	explicit PanelBackgroundImage(QQuickItem *parent = nullptr);

protected:
	void componentComplete() override;
	
private:
	void paint(QPainter *p) const override;
	void loadImage();
	void colorize(QImage &image) const;

	// Properties
	QUrl m_source;	
	bool m_colorize = false;	
	Plasma::Types::Location m_location = Plasma::Types::Floating;
	
	// The background image itself
	QImage m_background;
};
