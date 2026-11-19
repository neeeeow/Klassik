/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "removepanelmenu.h"

#include <KLocalizedString>

#include <Plasma/Containment>
#include <Plasma/Corona>

RemovePanelMenu::RemovePanelMenu(Plasma::Containment *containment, QWidget *parent)
	: PanelMenu(QIcon::fromTheme(QStringLiteral("list-remove")),
				i18n("Remove Panel"), containment, parent)
{
	if (containment) {
		if (const auto corona = containment->corona())
			connect(corona, &Plasma::Corona::containmentAdded, this, &PanelMenu::reinitialize);
	}
}

void
RemovePanelMenu::initialize()
{
	if (initialized()) return;

	// Clear out the list
	m_panels.clear();

	Plasma::Containment *containment = qobject_cast<Plasma::Containment *>(applet());
	if (!containment)
		return;

	Plasma::Corona *corona = containment->corona();
	if (!corona)
		return;

	for (Plasma::Containment *c : corona->containments()) {
		if (!c || c->destroyed() || c == containment)
			// We want to skip this containment when populating our list
			continue;

		const auto type = c->containmentType();
		if (type == Plasma::Containment::Panel || type == Plasma::Containment::CustomPanel) {
			// We need to reinitialize the panel if the containment is deleted
			connect(c, &Plasma::Applet::destroyedChanged, this, &PanelMenu::reinitialize, Qt::UniqueConnection);
			
			m_panels.append(c);
		}
	}
}

void
RemovePanelMenu::populate()
{
	if (!mayAddItem(AddToTarget::Panel))
		return;
	
	for (QPointer<Plasma::Containment> panel : m_panels) {
		if (!panel || panel->destroyed())
			continue;

		QString location;
		switch (panel->location()) {
		case Plasma::Types::TopEdge:
			location = i18n("Top");
			break;
		case Plasma::Types::BottomEdge:
			location = i18n("Bottom");
			break;
		case Plasma::Types::LeftEdge:
			location = i18n("Left");
			break;
		case Plasma::Types::RightEdge:
			location = i18n("Right");
			break;
		default:
			break;
		}
		const QString title = panel->title();
		const QString text = location.isEmpty() ? title : i18n("%1 (%2)", title, location);

		QAction *action = addAction(QIcon::fromTheme(panel->icon()), escapeMnemonics(text));
		connect(action, &QAction::triggered, this, [this, panel] {
			removePanel(panel);
		});
	}

	menuAction()->setEnabled(!isEmpty());
}

void
RemovePanelMenu::removePanel(Plasma::Containment *panel) const
{
	if (!panel)
		return;

	const auto type = panel->containmentType();
	if (type != Plasma::Containment::Panel && type != Plasma::Containment::CustomPanel) {
		qWarning() << "Attempted to remove non-panel containment!";
		return;
	}

	if (QAction *removeAction = panel->internalAction(QStringLiteral("remove")))
		removeAction->trigger();
	else
	    panel->destroy();
}
