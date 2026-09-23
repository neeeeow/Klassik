/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QQuickItem>
#include <QStyle>
#include <QStyleOption> // technically not needed here, but every subclass will use it, so include for convenience

class QImage;

class PaintedItem : public QQuickItem
{
	/* Template class for painting on to QML items with a QPainter. */

	Q_OBJECT
public:
	PaintedItem(QQuickItem *parent = nullptr);
	virtual ~PaintedItem() = default;	  

	Q_INVOKABLE void updateImage();

protected:
	void componentComplete() override;
	bool event(QEvent *event) override;
	void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;
	QSGNode *updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;
	void updatePolish() override;

	// Getter for the active QStyle
	QStyle* style() const { return m_style; }

private:
	QStyle *m_style;
	QImage m_paintedImage;

	// Loads the current QStyle
	void loadStyle();
	void styleChanged();

	// Paints the internal QImage used for drawing the control
	void paintControlToImage();

	// Function used for painting to the QQuickItem. This function must be implemented
	// by subclasses, and must *never* be called directly outside of paintControlToImage(),
	// otherwise things will break;
	virtual void paint(QPainter *painter) const = 0;
};
