/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <Plasma/Containment>

#include <QPalette>

#include <QtQuick/private/qquickpalette_p.h>

class QMimeData;

class PanelContainment : public Plasma::Containment
{
	Q_OBJECT
	// The custom QQuickPalette to be used by children of the containment
	Q_PROPERTY(QQuickPalette *palette READ palette NOTIFY paletteChanged FINAL)
	// Whether or not to colorize the palette
	Q_PROPERTY(bool colorize MEMBER m_colorize NOTIFY colorizeChanged FINAL)
public:
	PanelContainment(QObject *parentObject, const KPluginMetaData &data, const QVariantList &args);	

	bool eventFilter(QObject *watched, QEvent *event) override;

	// Returns the QQuickPalette used by the containment
	QQuickPalette *palette() const { return m_quickPalette; }

	// Custom mimedata handler. Adds custom icon applet as well as custom menu applet.
	// Returns true if it performed an action. If false, call ContainmentItem's processMimeData
	Q_INVOKABLE bool processMimeData(QMimeData *mimeData, int x, int y);
		
Q_SIGNALS:
	void paletteChanged();
	void colorizeChanged();

private:
	QPointer<QQuickPalette> m_quickPalette = nullptr;
	bool m_colorize = false;

	// Tints a given QColor according to the selection color
	QColor tintColor(const QColor &base);

private Q_SLOTS:
	// Loads and colorizes the active QPalette in to our QQuickPalette pointer
	void loadPalette();
};
