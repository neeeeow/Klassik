/*
 *  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid
import org.kde.plasma.clock

PlasmoidItem {
    id: root

    Plasmoid.onActivated: calendar.togglePopup()

    // Copy all sizing logic from mainLayout
    implicitWidth: mainLayout.implicitWidth
    implicitHeight: mainLayout.implicitHeight
    Layout.minimumWidth: mainLayout.Layout.minimumWidth
    Layout.minimumHeight: mainLayout.Layout.minimumHeight
    Layout.preferredWidth: mainLayout.Layout.preferredWidth
    Layout.preferredHeight: mainLayout.Layout.preferredHeight
    Layout.maximumWidth: mainLayout.Layout.maximumWidth
    Layout.maximumHeight: mainLayout.Layout.maximumHeight

    Clock {
        id: clock
        trackSeconds: Plasmoid.configuration.showSeconds
    }

    CalendarPopup {
        id: calendar
        location: Plasmoid.location
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

            sourceComponent: Plasmoid.configuration.useDigitalClock ? digitalComponent : analogComponent

            Component {
                id: digitalComponent
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
                        function onDateTimeChanged() {
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

            Component {
                id: analogComponent
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

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton

        onClicked: mouse => {
            calendar.togglePopup();
        }
    }

    PlasmaCore.ToolTipArea {
        anchors.fill: parent
        mainText: Plasmoid.configuration.use24hFormat ? Qt.formatTime(clock.dateTime, "HH:mm") : Qt.formatTime(clock.dateTime, "h:mm ap")
        subText: Qt.formatDate(clock.dateTime, Qt.locale().dateFormat(Locale.LongFormat))
        icon: Plasmoid.icon
    }
}
