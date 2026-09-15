/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "klassikpainteditem.h"

#include <QQuickWindow>
#include <QSGNode>
#include <QSGSimpleTextureNode>
#include <QPainter>

KlassikPaintedItem::KlassikPaintedItem(QQuickItem *parent) : QQuickItem(parent)
{
	setFlag(QQuickItem::ItemHasContents, true);
}

void
KlassikPaintedItem::componentComplete()
{
	QQuickItem::componentComplete();
    polish();
}

bool
KlassikPaintedItem::event(QEvent *event)
{
	if (event->type() == QEvent::ApplicationPaletteChange) {
	    updateImage();
	}

	return QQuickItem::event(event);
}

void
KlassikPaintedItem::updateImage()
{
	if (isComponentComplete())
		polish();
}

void
KlassikPaintedItem::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);
	updateImage();
}

void
KlassikPaintedItem::updatePolish()
{
	paintControlToImage();
}


QSGNode *
KlassikPaintedItem::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
{
    QSGSimpleTextureNode *node = static_cast<QSGSimpleTextureNode *>(oldNode);
	if (!node) {
		node = new QSGSimpleTextureNode();
		node->setOwnsTexture(true);
	}

	if (m_paintedImage.isNull()) {
        // If we cannot create a texture, the node should not exist either
        // because its material requires a texture.
        delete node;
        return nullptr;
    }

	const auto texture = window()->createTextureFromImage(m_paintedImage, QQuickWindow::TextureCanUseAtlas);

	// Bounding rect for texture
	QRectF bounds = boundingRect();

	// Correct bounds for fractional scaling
	const qreal dpr = window()->effectiveDevicePixelRatio();
	if (!qFuzzyCompare(dpr, qreal(1))) {
		QPointF windowPos = mapToScene(QPointF(0, 0));
		qreal physX = windowPos.x() * dpr;
		qreal physY = windowPos.y() * dpr;
		qreal fractionalX = physX - qFloor(physX);
		qreal fractionalY = physY - qFloor(physY);
		bounds.adjust(-fractionalX / dpr, -fractionalY / dpr, -fractionalX / dpr, -fractionalY / dpr);
	}

    node->setRect(bounds);
    node->setTexture(texture);
	
	return node;
}

void
KlassikPaintedItem::paintControlToImage()
{
	QSize imgSize = size().toSize();
	if (imgSize.isEmpty())    
		return;

	const qreal dpr = window()->effectiveDevicePixelRatio();
	imgSize *= dpr;

	if (m_paintedImage.size() != imgSize) {
		m_paintedImage = QImage(imgSize, QImage::Format_ARGB32_Premultiplied);
		m_paintedImage.setDevicePixelRatio(dpr);
	}

	m_paintedImage.fill(Qt::transparent);

	QPainter painter(&m_paintedImage);
	paint(&painter);
	
	update();
}
