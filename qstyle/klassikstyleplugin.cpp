#include "klassikstyleplugin.h"
#include "klassikstyle.h"

QStyle *KlassikStylePlugin::create(const QString &key)
{
	if (key.compare(QStringLiteral("Klassik"), Qt::CaseInsensitive) == 0)
		return new KlassikStyle(KlassikStyle::Default);
	else if (key.compare(QStringLiteral("HighColor Klassik"), Qt::CaseInsensitive) == 0)
		return new KlassikStyle(KlassikStyle::HighColor);
	else if (key.compare(QStringLiteral("B3/Klassik"), Qt::CaseInsensitive) == 0)
		return new KlassikStyle(KlassikStyle::B3);

	return nullptr;
}
