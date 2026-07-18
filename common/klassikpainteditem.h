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
	QSGNode *updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;
	void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;
	void updatePolish() override;

	virtual void paint(QPainter *painter) const = 0;

private:
	QImage m_paintedImage;

	inline QSize imageSize() { return size().toSize(); }

	bool event(QEvent *event) override;
	
	int dprAlignedSize(const int size) const;
	void paintControlToImage();
};
