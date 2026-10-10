/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QQuickItem>
#include <QPointer>

// We can technically forward declare these, but every subclass will need these
// so just include here for convenience.
#include <QStyle>
#include <QStyleOption>

class PaintedItem : public QQuickItem
{
	/* Template class for painting on to QML items with a QPainter. */

	Q_OBJECT
	QML_ANONYMOUS
	
public:
	explicit PaintedItem(QQuickItem *parent = nullptr);	 

	// Triggers a repaint of the QQuickItem
	Q_INVOKABLE void requestRepaint() {if (isComponentComplete()) polish();}
	
Q_SIGNALS:
	void propertyChanged();
	
protected:
	// Getter for the active QStyle. Whilst we do guard against it prior to painting
	// on to the internal QImage, it still might end up returning nullptr if the
	// style isn't loaded or has been destroyed before our signal fires, so be careful !
	QStyle* style() const { return m_style; }
	
	// Returns the QRect which represents the area to which the QQuickItem is painted on.
	// Only call from within paint(), m_paintedImage is only guaranteed to be non-null there.
	QRect rect() const { return QRect(QPoint(0, 0), m_paintedImage.deviceIndependentSize().toSize()); }

	// Returns the QRect which represents the pixel area to which the QQuickItem is painted on.
	// Only call from within paint(), m_paintedImage is only guaranteed to be non-null there.
	QRect scaledRect() const { return m_paintedImage.rect(); }

	// Scales a QPainter to allow for pixel perfect drawing
	static void scalePainter(QPainter *painter);
	
	// Overrides for painting logic
	void componentComplete() override;
	bool event(QEvent *event) override;
	void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;
	void updatePolish() override;
	QSGNode *updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;

private:
	QPointer<QStyle> m_style = nullptr; // Declare this a QPointer, since the QStyle might get destroyed by something else
	QImage m_paintedImage;

	// Loads the current QStyle
	void loadStyle();

	// Paints the internal QImage used for drawing the control
	void paintControlToImage();

	// Function used for painting to the QQuickItem. This function must be implemented
	// by subclasses, and must *never* be called directly outside of paintControlToImage(),
	// otherwise things will break;
	virtual void paint(QPainter *painter) const = 0;

private Q_SLOTS:
	void styleChanged();
};
