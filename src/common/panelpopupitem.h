/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QQuickItem>
#include <QVBoxLayout>
#include <Plasma/Plasma>

class QMenu;

class PanelPopupItem : public QQuickItem
{
	Q_OBJECT
	QML_ANONYMOUS
	Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
	Q_PROPERTY(Plasma::Types::Location location READ location WRITE setLocation)
public:
    explicit PanelPopupItem(QQuickItem *parent = nullptr);
	~PanelPopupItem() override;

	bool isActive() const;

	Plasma::Types::Location location() const { return m_location; }
	void setLocation(Plasma::Types::Location location) {
		if (m_location != location)
			m_location = location;
	}

	Q_INVOKABLE void togglePopup();

Q_SIGNALS:
	void activeChanged();

protected:	
	void componentComplete() override;

	// Clears all items of a QLayout.
	void clearLayout(QLayout *layout);

	// Called whenever the popup is about to show. Override this function to set custom behaviour.
	virtual void aboutToShow() {}

private:
	// Stores the location of the panel
	Plasma::Types::Location m_location = Plasma::Types::Floating;
	
	// Points to the QMenu which stores the contents of our popup. Must *never* be
	// accessed outside of this class. Use a QPointer in case something else
	// destroys the menu
	QPointer<QMenu> m_menu = nullptr;

	// The layout to which we add items
	QVBoxLayout *m_layout = nullptr;

	// Determines the position at which to display the dialog
	QPoint popupPosition() const;
	
	// Initializes the contents of the popup (within the QWidget/QVBoxLayout)
	virtual void initContents(QVBoxLayout *layout) = 0;
};
