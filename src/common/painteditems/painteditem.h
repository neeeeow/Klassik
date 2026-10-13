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

	// Custom state flags which correspond to QStyle::State_Sunken and
	// QStyle::State_MouseOver. By default, both flags are off. Note: we
	// make hovered read only since it must never be set outside of our C++ code
	Q_PROPERTY(bool sunken MEMBER m_sunken NOTIFY propertyChanged FINAL)
	Q_PROPERTY(bool hovered READ isHovered NOTIFY propertyChanged FINAL)
	Q_PROPERTY(bool hoverEnabled READ isHoverEnabled WRITE setHoverEnabled NOTIFY propertyChanged FINAL)
	
public:
	explicit PaintedItem(QQuickItem *parent = nullptr);

	bool isHovered() const { return m_hovered; }
	bool isHoverEnabled() const { return m_hoverEnabled; }
	void setHoverEnabled(bool hoverEnabled) {
		if (hoverEnabled != m_hoverEnabled) {
			m_hoverEnabled = hoverEnabled;
			if (!m_hoverEnabled)
				m_hovered = false;			
			setAcceptHoverEvents(m_hoverEnabled);
			Q_EMIT propertyChanged();
		}
	}
	
Q_SIGNALS:
	// Signal emitted whenever an internal property (both exposed and hidden from QML, such as
	// state flags, or palette, or active QStyle) is changed. By default, this signal is connected
	// to requestRepaint().
	void propertyChanged();
	
protected:
	// Getter for the active QStyle. Whilst we do guard against it prior to painting
	// on to the internal QImage, it still might end up returning nullptr if the
	// style isn't loaded or has been destroyed before our signal fires, so be careful !
	QStyle* style() const { return m_style; }

	// Returns a basic QStyleOption with state flags already set, for convenience.
	QStyleOption baseStyleOption() const; 

	// Returns state flags used by QStyle
	bool isSunken() const { return m_sunken; }
	
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
	void updatePolish() override;
	QSGNode *updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;

	// Event handling overrides
	bool event(QEvent *event) override;
	void itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value) override;
	void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;
	void hoverEnterEvent(QHoverEvent *event) override;
	void hoverLeaveEvent(QHoverEvent *event) override;
														
protected Q_SLOTS:	
	// Triggers a repaint of the QQuickItem
	void requestRepaint() {if (isComponentComplete()) polish();}
	
private:
	QPointer<QStyle> m_style = nullptr; // Declare this a QPointer, since the QStyle might get destroyed by something else

	// QImage to which contents are painted on
	QImage m_paintedImage;

	// Loads the current QStyle
	void loadStyle();

	// State flags used by QStyle
	bool m_sunken = false;
	bool m_hovered = false;

	// Whether or not to accept hover events (default false!)
	bool m_hoverEnabled = false;
	
	// Function used for painting to the QQuickItem. This function must be implemented
	// by subclasses, and must *never* be called directly outside of updatePolish(),
	// otherwise things will break;
	virtual void paint(QPainter *painter) const = 0;

private Q_SLOTS:
	void styleChanged();
};
