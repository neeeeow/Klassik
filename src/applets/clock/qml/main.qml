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

    Clock {
        id: clock
        trackSeconds: Plasmoid.configuration.showSeconds
    }

    CalendarPopup {
        id: calendar
        location: Plasmoid.location
    }

    preferredRepresentation: fullRepresentation
    fullRepresentation: Item {
        // Copy all sizing logic from mainLayout
        implicitWidth: mainLayout.implicitWidth
        implicitHeight: mainLayout.implicitHeight
        Layout.minimumWidth: mainLayout.Layout.minimumWidth
        Layout.minimumHeight: mainLayout.Layout.minimumHeight
        Layout.preferredWidth: mainLayout.Layout.preferredWidth
        Layout.preferredHeight: mainLayout.Layout.preferredHeight
        Layout.maximumWidth: mainLayout.Layout.maximumWidth
        Layout.maximumHeight: mainLayout.Layout.maximumHeight

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

                readonly property int numDigits: item?.timeString?.length ?? 0
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

                        property string timeString: { // String containing the digits for the clock
                            if (dotsVisible || (!blinkingDots)) {
                                return Qt.formatTime(clock.dateTime, Plasmoid.configuration.showSeconds ? "HH:mm:ss" : "HH:mm");
                            } else {
                                return Qt.formatTime(clock.dateTime, Plasmoid.configuration.showSeconds ? "HH mm ss" : "HH mm");
                            }
                        }
                        text: timeString

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
                        showSeconds: Plasmoid.configuration.showSeconds
                        showFrame: Plasmoid.configuration.showFrame
                        antialiasing: Plasmoid.configuration.antialiasing
                        colorTheme: Plasmoid.configuration.colorTheme
                        fgColor: Plasmoid.configuration.fgColor
                        shadowColor: Plasmoid.configuration.shadowColor
                        bgColor: Plasmoid.configuration.bgColor
                        Connections {
                            target: clock
                            function onDateTimeChanged() {
                                analogClock.requestRepaint()
                            }
                        }
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
            id: mouseArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton

            onClicked: mouse => {
                if (mouse.button == Qt.LeftButton) {
                    calendar.togglePopup();
                }
            }

            PlasmaCore.ToolTipArea {
                anchors.fill: parent
                mainText: Qt.formatTime(clock.dateTime, "HH:mm");
                subText: Qt.formatDate(clock.dateTime, Qt.locale().dateFormat(Locale.LongFormat))
                icon: Plasmoid.icon
            }
        }
    }
}
