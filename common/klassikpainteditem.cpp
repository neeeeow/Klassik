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

void
KlassikPaintedItem::updateImage()
{
	if (isComponentComplete())
		polish();
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
KlassikPaintedItem::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);
	updateImage();
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
KlassikPaintedItem::updatePolish()
{
	paintControlToImage();
}

int
KlassikPaintedItem::dprAlignedSize(const int size) const
{
    // Return the first value equal to or bigger than size
    // that is a whole number when multiplied with the dpr.
    static int multiplier = [&]() {
        const qreal dpr = window()->effectiveDevicePixelRatio();
        for (int m = 1; m <= 10; ++m) {
            const qreal v = m * dpr;
            if (v == int(v))
                return m;
        }

        qWarning() << "The current dpr (" << dpr << ") is not supported"
                   << "by the style and might result in drawing artifacts";
        return 1;
    }();

    return int(qCeil(qreal(size) / qreal(multiplier)) * multiplier);
}

void
KlassikPaintedItem::paintControlToImage()
{
	const QSize imgSize = imageSize();
	if (imgSize.isEmpty())    
		return;

	const qreal dpr = window()->effectiveDevicePixelRatio();
    const int alignedW = int(dprAlignedSize(imgSize.width()) * dpr);
    const int alignedH = int(dprAlignedSize(imgSize.height()) * dpr);
    const QSize alignedSize = QSize(alignedW, alignedH);

	if (m_paintedImage.size() != alignedSize) {
		m_paintedImage = QImage(alignedSize, QImage::Format_ARGB32_Premultiplied);
		m_paintedImage.setDevicePixelRatio(dpr);
	}

	m_paintedImage.fill(Qt::transparent);

	QPainter painter(&m_paintedImage);
	paint(&painter);
	
	update();
}
