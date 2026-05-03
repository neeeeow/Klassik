#include "kmenuplugin.h"
#include "kmenu.h"

void
KMenuPlugin::registerTypes(const char *uri)
{
	Q_ASSERT(QLatin1String(uri) == QLatin1String("com.github.neeeeow.klassik.kmenu"));

	qmlRegisterType<KMenu>(uri, 1, 0, "KMenu");
}
