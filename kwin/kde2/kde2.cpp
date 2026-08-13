#include "kde2.h"

#include <KPluginFactory>

#include <QPainter>

K_PLUGIN_FACTORY_WITH_JSON(
	KDE2DecorationFactory,
	"metadata.json",
	registerPlugin<KDE2Decoration>();
	)

KDE2Decoration::KDE2Decoration(QObject *parent, const QVariantList &args) : KDecoration3::Decoration(parent, args)
{
}

bool
KDE2Decoration::init()
{
	return true;
}

void
KDE2Decoration::paint(QPainter *p, const QRectF &repaintRegion)
{

}

#include "kde2.moc"
