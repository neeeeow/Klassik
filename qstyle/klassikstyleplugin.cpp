#include "klassikstyleplugin.h"
#include "klassikstyle.h"

QStyle *KlassikStylePlugin::create(const QString &key)
{
	if (key.compare(QStringLiteral("Klassik"), Qt::CaseInsensitive) == 0) {
		return new KlassikStyle();
	}

	return nullptr;
}
