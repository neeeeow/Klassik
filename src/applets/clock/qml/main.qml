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

    preferredRepresentation: fullRepresentation
    fullRepresentation: ColumnLayout {
        id: mainLayout
        spacing: 0

        readonly property bool isHorizontal: Plasmoid.formFactor === PlasmaCore.Types.Horizontal
        readonly property bool isVertical: Plasmoid.formFactor === PlasmaCore.Types.Vertical

        Layout.fillWidth: isVertical
        Layout.fillHeight: isHorizontal

        Layout.preferredWidth: {
            var _showSeconds = Plasmoid.configuration.showSeconds; // forces geometry to be recomputed if we change second config
            if (isHorizontal) {
                return Math.max(clockLoader.item ? clockLoader.item.preferredWidthForHeight(clockLoader.item.height) : 0, date.visible ? date.implicitWidth : 0);
            } else {
                return -1;
            }
        }

        Layout.preferredHeight: {
            var _showSeconds = Plasmoid.configuration.showSeconds;
            if (isVertical) {
                return (clockLoader.item ? clockLoader.item.preferredHeightForWidth(clockLoader.item.width) : 0) + (date.visible ? date.implicitHeight : 0);
            } else {
                return -1;
            }
        }

        // Ensure the width/height *never* deviates from our computed values
        Layout.minimumWidth: isHorizontal ? Layout.preferredWidth : -1
        Layout.maximumWidth: isHorizontal ? Layout.preferredWidth : -1
        Layout.minimumHeight: isVertical ? Layout.preferredHeight : -1
        Layout.maximumHeight: isVertical ? Layout.preferredHeight : -1

        Loader {
            id: clockLoader
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            Layout.fillHeight: true

            sourceComponent: Plasmoid.configuration.useDigitalClock ? digitalComponent : analogComponent

            Binding {
                target: clockLoader.item
                property: "config"
                value: Plasmoid.configuration
                when: clockLoader.item
            }

            Component {
                id: digitalComponent
                DigitalClock {
                    id: digitalClock
                    readonly property bool blinkingDots: Plasmoid.configuration.blinkingDots
                    property bool dotsVisible: true // whether or not the dots are visible *if* blinkingDots is enabled

                    text: {
                        if (dotsVisible || (!blinkingDots)) {
                            return Qt.formatTime(clock.dateTime, Plasmoid.configuration.showSeconds ? "HH:mm:ss" : "HH:mm");
                        } else {
                            return Qt.formatTime(clock.dateTime, Plasmoid.configuration.showSeconds ? "HH mm ss" : "HH mm");
                        }
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
                    Connections {
                        target: clock
                        function onDateTimeChanged() {
                            analogClock.updateImage()
                        }
                    }
                }
            }
        }

        Label {
            id: date
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 8
            visible: Plasmoid.configuration.showDate
            text: Qt.formatDate(clock.dateTime, Qt.locale().dateFormat(Locale.ShortFormat))
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton

            PlasmaCore.ToolTipArea {
                anchors.fill: parent
                mainText: Qt.formatTime(clock.dateTime, "HH:mm");
                subText: Qt.formatDate(clock.dateTime, Qt.locale().dateFormat(Locale.LongFormat))
                icon: "preferences-system-time"
            }
        }
    }
}
