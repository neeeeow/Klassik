/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
 */
pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.extras as PlasmaExtras
import org.kde.plasma.plasmoid
import org.kde.plasma.clock
import org.kde.kcmutils as KCMUtils
import org.kde.config as KConfig

import com.github.neeeeow.klassik.actionfactory

PlasmoidItem {
    id: root

// BEGIN properties

    Plasmoid.onActivated: calendar.open()

    // Copy all sizing logic from mainLayout
    implicitWidth: mainLayout.implicitWidth
    implicitHeight: mainLayout.implicitHeight
    Layout.minimumWidth: mainLayout.Layout.minimumWidth
    Layout.minimumHeight: mainLayout.Layout.minimumHeight
    Layout.preferredWidth: mainLayout.Layout.preferredWidth
    Layout.preferredHeight: mainLayout.Layout.preferredHeight
    Layout.maximumWidth: mainLayout.Layout.maximumWidth
    Layout.maximumHeight: mainLayout.Layout.maximumHeight

// END properties

// BEGIN connections

    Connections {
        target: Plasmoid.configuration
        function onSelectedTimeZonesChanged(): void {
            if (Plasmoid.configuration.selectedTimeZones.indexOf(Plasmoid.configuration.lastSelectedTimezone) === -1) {
                Plasmoid.configuration.lastSelectedTimezone = Plasmoid.configuration.selectedTimeZones[0] ?? "Local";
            }
        }
    }

    Connections {
        target: Plasmoid
        function onContextualActionsAboutToShow(): void {
            // Fill the time zone menu
            timeZoneMenu.clearMenuItems();
            const zones = Plasmoid.configuration.selectedTimeZones;
            for (let i = 0; i < zones.length; ++i) {
                const zone = zones[i];
                const item = root.newMenuItem(timeZoneMenu);
                item.text = zone === "Local" ? i18n("Local Timezone") : zone;
                item.checkable = true;
                item.checked = (Plasmoid.configuration.lastSelectedTimezone === zone);
                item.clicked.connect(() => {
                    Plasmoid.configuration.lastSelectedTimezone = zone;
                });
            }

            // Update the clipboard menu's properties
            ClipboardMenu.secondsIncluded = Plasmoid.configuration.showSeconds;
            ClipboardMenu.timezone = clock.timeZone;
        }
    }

    Component.onCompleted: {
        ClipboardMenu.setupMenu(clipboardAction); // Link the clipboard menu to its action
        Plasmoid.contextualActions = [
            ActionFactory.popupTitle("clock", i18n("Clock"), root),
            clockTypeAction,
            timeZoneAction,
            clipboardAction, // technically, KDE 3 had this action at the bottom, but this makes more sense
            adjustDateTimeAction
        ];
    }

// END connections

// BEGIN components

    readonly property Component digitalComponent: Component {
        DigitalClock {
            id: digitalClock
            showFrame: Plasmoid.configuration.showFrame
            colorTheme: Plasmoid.configuration.colorTheme
            fgColor: Plasmoid.configuration.fgColor
            shadowColor: Plasmoid.configuration.shadowColor
            bgColor: Plasmoid.configuration.bgColor

            readonly property bool blinkingDots: Plasmoid.configuration.blinkingDots
            property bool dotsVisible: true // whether or not the dots are visible *if* blinkingDots is enabled

            text: { // String containing the digits for the clock
                const separator = (dotsVisible || !blinkingDots) ? ":" : " ";
                const seconds = Plasmoid.configuration.showSeconds ? separator + "ss" : "";
                const format = Plasmoid.configuration.use24hFormat
                ? "HH" + separator + "mm" + seconds
                : "h" + separator+ "mm" + seconds + "|AP";
                return Qt.formatTime(clock.dateTime, format).split("|")[0];
            }

            Timer {
                id: blinkingDotsTimer
                interval: 500
                repeat: true
                running: digitalClock.blinkingDots
                onTriggered: digitalClock.dotsVisible = !digitalClock.dotsVisible
            }

            Connections {
                target: clock
                function onDateTimeChanged(): void {
                    if (digitalClock.blinkingDots) {
                        // Start each time update with the dots visible. This is technically not 100% accurate per KDE 3 behaviour, but
                        // it looks way more consistent.
                        digitalClock.dotsVisible = true;
                        blinkingDotsTimer.restart();
                    }
                }
            }
        }
    }

    readonly property Component analogComponent: Component {
        AnalogClock {
            id: analogClock
            antialiasing: Plasmoid.configuration.antialiasing

            showSeconds: Plasmoid.configuration.showSeconds
            showFrame: Plasmoid.configuration.showFrame
            colorTheme: Plasmoid.configuration.colorTheme
            fgColor: Plasmoid.configuration.fgColor
            shadowColor: Plasmoid.configuration.shadowColor
            bgColor: Plasmoid.configuration.bgColor

            dateTime: clock.dateTime
        }
    }

    readonly property Component _menuItemComponent:  Component { PlasmaExtras.MenuItem {} }
    function newMenuItem(parent: QtObject): PlasmaExtras.MenuItem  { return _menuItemComponent.createObject(parent) as PlasmaExtras.MenuItem }

// END components

// BEGIN UI elements

    Clock {
        id: clock
        timeZone: Plasmoid.configuration.lastSelectedTimezone
        trackSeconds: Plasmoid.configuration.showSeconds
    }

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        spacing: 0

        Loader {
            id: clockLoader

            readonly property bool horizontal: Plasmoid.formFactor === PlasmaCore.Types.Horizontal
            readonly property bool vertical: Plasmoid.formFactor === PlasmaCore.Types.Vertical
            readonly property bool floating: (!horizontal) && (!vertical)

            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: vertical || floating
            Layout.fillHeight: horizontal || floating

            readonly property int numDigits: item?.text?.length ?? 0
            Layout.preferredWidth: {
                numDigits;
                if (horizontal) {
                    return clockLoader.item ? clockLoader.item.preferredWidthForHeight(clockLoader.item.height) : 0;
                } else {
                    return -1;
                }
            }
            Layout.preferredHeight: {
                numDigits;
                if (vertical) {
                    return clockLoader.item ? clockLoader.item.preferredHeightForWidth(clockLoader.item.width) : 0;
                } else {
                    return -1;
                }
            }

            // Ensure the width/height *never* deviates from our computed values
            Layout.minimumWidth: horizontal ? Layout.preferredWidth : -1
            Layout.maximumWidth: horizontal ? Layout.preferredWidth : -1
            Layout.minimumHeight: vertical ? Layout.preferredHeight : -1
            Layout.maximumHeight: vertical ? Layout.preferredHeight : -1

            // use digital clock by default!!
            sourceComponent: Plasmoid.configuration.clockType === 1 ? root.analogComponent : root.digitalComponent
        }

        Label {
            id: date
            Layout.alignment: Qt.AlignCenter
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 8
            visible: Plasmoid.configuration.showDate
            text: Qt.formatDate(clock.dateTime, Qt.locale().dateFormat(Locale.ShortFormat))
        }
    }

    // Tooltip area
    PlasmaCore.ToolTipArea {
        anchors.fill: parent
        mainText: Plasmoid.configuration.use24hFormat ? Qt.formatTime(clock.dateTime, "HH:mm") : Qt.formatTime(clock.dateTime, "h:mm ap")
        subText: Qt.formatDate(clock.dateTime, Qt.locale().dateFormat(Locale.LongFormat))
        icon: Plasmoid.icon
    }

    // Calendar popup
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton

        onClicked: mouse => {
            calendar.open(); // use open() here since we don't need it to open relative to anything
        }
    }
    PlasmaExtras.Menu {
        id: calendar

        PlasmaExtras.MenuItem {
            id: calendarTitle
            Component.onCompleted: action = ActionFactory.popupTitle("office-calendar", i18n("Calendar"), calendarTitle)
        }
        PlasmaExtras.MenuItem {
            id: calendarItem
            Component.onCompleted: action = ActionFactory.popupCalendar(calendarItem)
        }
    }

    // Select clock type menu
    PlasmaCore.Action {
        id: clockTypeAction
        text: i18n("Type")
    }
    PlasmaExtras.Menu {
        visualParent: clockTypeAction

        PlasmaExtras.MenuItem {
            text: i18n("Digital")
            checkable: true
            checked: Plasmoid.configuration.clockType === 0
            onClicked: Plasmoid.configuration.clockType = 0
        }

        PlasmaExtras.MenuItem {
            text: i18n("Analog")
            checkable: true
            checked: Plasmoid.configuration.clockType === 1
            onClicked: Plasmoid.configuration.clockType = 1
        }
    }

    // Select time zone menu
    PlasmaCore.Action {
        id: timeZoneAction
        text: i18n("Show Time&zone")
        icon.name: "globe" // KDE 3 never had an icon for this, but we improvise
    }
    PlasmaExtras.Menu {
        id: timeZoneMenu
        visualParent: timeZoneAction
    }

    // Copy to clipboard action
    PlasmaCore.Action {
        id: clipboardAction
        text: i18n("Copy to Clipboard")
        icon.name: "edit-copy"
    }

    // kcm_clock action
    PlasmaCore.Action {
        id: adjustDateTimeAction
        text: i18n("Adjust Date && Time…")
        icon.name: "preferences-system-time"
        visible: KConfig.KAuthorized.authorizeControlModule("kcm_clock.desktop")
        onTriggered: KCMUtils.KCMLauncher.open(["kcm_clock"])
    }

// END UI elements
}
