/*
  SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <Plasma/Containment>

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

	// Returns all of the contextual actions (both created in C++ and QML) for a given applet.
	Q_INVOKABLE static QList<QAction *> allContextualActions(Plasma::Applet *applet);

public Q_SLOTS:
	void init() override;
	
Q_SIGNALS:
	void paletteChanged();
	void colorizeChanged();

private:
	// Tints a given QColor according to the selection color
	static QColor tintColor(const QColor &base);

	// Creates the containment's internal actions
	void createInternalActions();

    QQuickPalette *m_quickPalette = nullptr;
	bool m_colorize = false;

private Q_SLOTS:
	// Loads and colorizes the active QPalette in to our QQuickPalette pointer
	void loadPalette();
};
