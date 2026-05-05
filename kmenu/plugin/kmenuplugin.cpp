#include "kmenuplugin.h"
#include "kmenumanager.h"

void
KMenuPlugin::registerTypes(const char *uri)
{
	Q_ASSERT(QLatin1String(uri) == QLatin1String("com.github.neeeeow.klassik.kmenu"));

	qmlRegisterType<KMenuManager>(uri, 1, 0, "KMenuManager");
}
