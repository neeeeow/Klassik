/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "painteditem.h"

#include <QQuickWindow>
#include <QSGNode>
#include <QSGSimpleTextureNode>
#include <QPainter>
#include <QApplication>
#include <QImage>

PaintedItem::PaintedItem(QQuickItem *parent) : QQuickItem(parent)
{
	setFlag(QQuickItem::ItemHasContents, true);
	loadStyle();
}

void
PaintedItem::loadStyle()
{	
	m_style = qApp->style();
	// We cannot connect to QEvent::StyleChange, so we do it this way instead
	if (m_style)
		connect(m_style, &QObject::destroyed, this, &PaintedItem::styleChanged, Qt::UniqueConnection);
}	
void
PaintedItem::styleChanged()
{
	if (QCoreApplication::closingDown())
		return;
	loadStyle();
	if (m_style)
		updateImage();
}

void
PaintedItem::componentComplete()
{
	QQuickItem::componentComplete();
    polish();
}

void
PaintedItem::updateImage()
{
	if (isComponentComplete())
		polish();
}

bool
PaintedItem::event(QEvent *event)
{
	if (event->type() == QEvent::ApplicationPaletteChange)
	    updateImage();

	return QQuickItem::event(event);
}

void
PaintedItem::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChange(newGeometry, oldGeometry);
	updateImage();
}

void
PaintedItem::updatePolish()
{
	QQuickItem::updatePolish();
	paintControlToImage();
}


QSGNode *
PaintedItem::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
{
	Q_UNUSED(updatePaintNodeData);
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
		const QPointF windowPos = mapToScene(QPointF(0, 0));
		const qreal physX = windowPos.x() * dpr;
		const qreal physY = windowPos.y() * dpr;
		const qreal fractionalX = physX - qFloor(physX);
		const qreal fractionalY = physY - qFloor(physY);
		bounds.adjust(-fractionalX / dpr, -fractionalY / dpr, -fractionalX / dpr, -fractionalY / dpr);
	}

    node->setRect(bounds);
    node->setTexture(texture);
	
	return node;
}

void
PaintedItem::paintControlToImage()
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
