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
	connect(this, &PaintedItem::propertyChanged, this, &PaintedItem::requestRepaint);
}

void
PaintedItem::loadStyle()
{	
	m_style = QApplication::style();
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
		requestRepaint();
}

void
PaintedItem::scalePainter(QPainter *painter)
{
	if (!painter)
		return;
	const qreal dpr = painter->device() ? painter->device()->devicePixelRatio() : 1.0;
	if (!qFuzzyCompare(dpr, qreal(1))) {
		const qreal inverseScale = qreal(1) / dpr;
		painter->scale(inverseScale, inverseScale);
	}
}

void
PaintedItem::componentComplete()
{
	QQuickItem::componentComplete();
	polish();
}

bool
PaintedItem::event(QEvent *event)
{	
	if (event->type() == QEvent::ApplicationPaletteChange)
		requestRepaint();

	return QQuickItem::event(event);
}

void
PaintedItem::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
	QQuickItem::geometryChange(newGeometry, oldGeometry);

	// We only need to repaint the image if the geometry size changes ...
	if (newGeometry.size() != oldGeometry.size())
		requestRepaint();
	// ... however, we will need to update the paint node if
	// position changes, since our HiDPI corrections depend on position
	else if (newGeometry.topLeft() != oldGeometry.topLeft())
		update();
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
    auto *node = static_cast<QSGSimpleTextureNode *>(oldNode);

	if (m_paintedImage.isNull() || !window()) {
		// If we cannot create a texture or there is no window to draw on,
		// the node should not exist.
		delete node;
		return nullptr;
	}

	auto *texture = window()->createTextureFromImage(m_paintedImage, QQuickWindow::TextureCanUseAtlas);
	if (!texture) {
		delete node;
		return nullptr;
	}

	if (!node) {
		node = new QSGSimpleTextureNode();
		node->setOwnsTexture(true);
	}

	// Bounding rect for texture
	QRectF bounds = boundingRect();

	// Snap the top left corner to the nearest pixel
	const qreal dpr = window()->effectiveDevicePixelRatio();
	const QPointF scenePos = mapToScene(QPointF(0,0)); // Top left of the window
	const QPointF adjustedScenePos( // Top left pixel in the window
		qRound(scenePos.x() * dpr) / dpr,
		qRound(scenePos.y() * dpr) / dpr
		);	 
	bounds.translate(adjustedScenePos - scenePos);

	node->setRect(bounds);
	node->setTexture(texture);
	
	return node;
}

void
PaintedItem::paintControlToImage()
{			
	if (size().isEmpty() || !window() || !m_style) {
		m_paintedImage = QImage(); // Clear out the QImage if invalid
		update();
		return;
	}

	const qreal dpr = window()->effectiveDevicePixelRatio();
	const QSize imgSize(qRound(width() * dpr), qRound(height() * dpr));

	if (m_paintedImage.size() != imgSize) {
		m_paintedImage = QImage(imgSize, QImage::Format_ARGB32_Premultiplied);
		m_paintedImage.setDevicePixelRatio(dpr);
	}

	m_paintedImage.fill(Qt::transparent);

	QPainter painter(&m_paintedImage);
	paint(&painter);
	
	update();
}
