#pragma once

#include <KDecoration3/Decoration>
#include <KDecoration3/DecorationButton>
#include <KDecoration3/DecorationButtonGroup>

#include <QVariant>
#include <QPixmap>
#include <QBitmap>

class KDE2Decoration : public KDecoration3::Decoration
{
	Q_OBJECT
public:
	explicit KDE2Decoration(QObject *parent = nullptr, const QVariantList &args = QVariantList());
	~KDE2Decoration() override = default;
	
	bool init() override;
	void paint(QPainter *p, const QRectF &repaintRegion) override;

private:
	qreal m_titleHeight = 14;
	int m_borderWidth = 4;
	int m_grabBorderWidth = 8;

	QPixmap titlePix;
			
	KDecoration3::DecorationButtonGroup *m_leftButtons = nullptr;
	KDecoration3::DecorationButtonGroup *m_rightButtons = nullptr;

	void reconfigure();
	void updateBorders();
	void updateButtonsGeometry();
	void updateTitleBar();
	void updateButtonsGeometryDelayed();
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
