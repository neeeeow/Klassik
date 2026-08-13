#pragma once

#include <KDecoration3/DecoratedWindow>
#include <KDecoration3/Decoration>
#include <KDecoration3/DecorationButton>
#include <KDecoration3/DecorationButtonGroup>

#include <QVariant>

class KDE2Decoration : public KDecoration3::Decoration
{
	Q_OBJECT
public:
	explicit KDE2Decoration(QObject *parent = nullptr, const QVariantList &args = QVariantList());
	~KDE2Decoration() override = default;
	
	bool init() override;
	void paint(QPainter *p, const QRectF &repaintRegion) override;
};
