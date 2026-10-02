/*
  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

  SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QQuickItem>
#include <QFrame>
#include <QVBoxLayout>
#include <Plasma/Plasma>

class PanelPopupItem : public QQuickItem
{
	Q_OBJECT
	Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
	Q_PROPERTY(Plasma::Types::Location location READ location WRITE setLocation)
public:
    PanelPopupItem(QQuickItem *parent = nullptr);
	~PanelPopupItem() override;

	bool isActive() const { return m_frame && m_frame->isVisible(); }

	Plasma::Types::Location location() const { return m_location; }
	void setLocation(Plasma::Types::Location location) {
		if (m_location != location)
			m_location = location;
	}

	Q_INVOKABLE void togglePopup();

Q_SIGNALS:
	void activeChanged();

protected:
    QFrame* frame() const { return m_frame; }
	
	void componentComplete() override;
	bool eventFilter(QObject *watched, QEvent *event) override;

	// Called whenever the popup is about to show. Override this function to set custom behaviour.
	virtual void aboutToShow() {};
	
	// Resets the margins of the layout containing the items
	void resetMargins();

private:
	// Whether or not the popup contents have been initialized.
	bool m_initialized = false;
	
	// Stores the location of the panel
	Plasma::Types::Location m_location = Plasma::Types::Floating;
	
	// Points to the QFrame and QLayout objects which holds the popup contents.
	// m_frame must only ever be accessed out of this class for parenting
	QPointer<QFrame> m_frame = nullptr;
	QVBoxLayout *m_layout = nullptr;

	// Reset margins in case of a QStyle change
	void styleChanged();

	// Determines the position at which to display the dialog
	QPoint popupPosition() const;
	
	// Initializes the contents of the popup (within the QFrame/QGridLayout)
	virtual void initContents(QVBoxLayout *layout) = 0;
};
