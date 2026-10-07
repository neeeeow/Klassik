/*
   SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

   SPDX-FileCopyrightText: 1999-2001 Daniel Duley <mosfet@kde.org>
   SPDX-FileCopyrightText: 1999-2001 Matthias Ettrich <ettrich@kde.org>
   SPDX-FileCopyrightText: 1999-2001 Karol Szwed <gallium@kde.org>
  
   SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <KDecoration3/Decoration>
#include <KDecoration3/DecorationButton>
#include <KDecoration3/DecorationButtonGroup>

#include <QVariant>
#include <QBitmap>

struct KDE2Pixmaps
{
	// Titlebar stipple
	QPixmap titlePix;

	// Buttons
	QPixmap pinDownPix;
	QPixmap pinUpPix;

	QPixmap leftBtnUpPix;
	QPixmap leftBtnDownPix;
	QPixmap ileftBtnUpPix;
	QPixmap ileftBtnDownPix;

	QPixmap rightBtnUpPix;
	QPixmap rightBtnDownPix;
	QPixmap irightBtnUpPix;
	QPixmap irightBtnDownPix;
};

class KDE2Decoration : public KDecoration3::Decoration
{
	Q_OBJECT
public:
	explicit KDE2Decoration(QObject *parent = nullptr, const QVariantList &args = QVariantList());
	
	bool init() override;
	void paint(QPainter *p, const QRectF &repaintRegion) override;

	// Access pixmaps from outside of the decoration class (i.e. inside the button)
	const KDE2Pixmaps &pixmaps() const { return m_pixmaps; }

private:
	KDE2Pixmaps m_pixmaps;
	qreal m_titleHeight = 14;
	int m_borderWidth = 4;
	int m_grabBorderWidth = 8;	
			
	KDecoration3::DecorationButtonGroup *m_leftButtons = nullptr;
	KDecoration3::DecorationButtonGroup *m_rightButtons = nullptr;

	void reconfigure();
	void updateBorders();
	void updateButtonsGeometry();
	void updateTitleBar();
	void createPixmaps();
};


class KDE2Button : public KDecoration3::DecorationButton
{
	Q_OBJECT

public:
	explicit KDE2Button(KDecoration3::DecorationButtonType type,
						KDecoration3::DecorationButtonGroup::Position position,
						KDecoration3::Decoration *decoration,
						QObject *parent = nullptr);
	~KDE2Button() override = default;

	static KDE2Button *create(KDecoration3::DecorationButtonType type,
							  KDecoration3::DecorationButtonGroup::Position position,
							  KDecoration3::Decoration *decoration,
							  QObject *parent);

	void paint(QPainter *p, const QRectF &repaintRegion) override;

private:
	KDecoration3::DecorationButtonGroup::Position m_position;
	QBitmap iconBits;

	void setIconBits();

	inline bool isLeft() const {
		return m_position == KDecoration3::DecorationButtonGroup::Position::Left;
	}
};
