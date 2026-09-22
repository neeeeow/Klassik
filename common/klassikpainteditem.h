/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QQuickItem>
#include <QImage>

class KlassikPaintedItem : public QQuickItem
{
	/* Template class for painting on to QML items with a QPainter. */

	Q_OBJECT
public:
	KlassikPaintedItem(QQuickItem *parent = nullptr);
	virtual ~KlassikPaintedItem() = default;	  

	Q_INVOKABLE void updateImage();

protected:
	void componentComplete() override;
	bool event(QEvent *event) override;
	void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;
	QSGNode *updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;
	void updatePolish() override;   

private:
	QImage m_paintedImage;
	void paintControlToImage();

	// Function used for painting to the QQuickItem. This function must be implemented
	// by subclasses, and must *never* be called directly outside of paintControlToImage(),
	// otherwise things will break;
	virtual void paint(QPainter *painter) const = 0;
};
