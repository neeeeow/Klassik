import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid

PlasmoidItem {
    id: root

    property string timeString: "99:99" // use a default value just in case
    property string dateString: ""
    Timer { // Internal timer used for keeping track of our clock
        id: clockTimer
        interval: 500
        running: true
        repeat: true
        triggeredOnStart: true

        property bool showDots: true

        onTriggered: {
            var time = new Date();

            clockTimer.showDots = !clockTimer.showDots;
            var hourString = Qt.formatTime(time, "hh");
            var minuteString = Qt.formatTime(time, "mm");
            var secondString = Qt.formatTime(time, "ss");
            var separator = Plasmoid.configuration.blinkingDots ? (clockTimer.showDots ? ":" : " ") : ":";

            var timeString;
            if (Plasmoid.configuration.showSeconds) {
                timeString = hourString + separator + minuteString + separator + secondString;
            } else {
                timeString = hourString + separator + minuteString;
            }

            if (root.timeString !== timeString)
                root.timeString = timeString;

            var dateString = Qt.formatDate(time, "dd/MM/yyyy");
            if (root.dateString !== dateString)
                root.dateString = dateString;
        }
    }

    preferredRepresentation: fullRepresentation
    fullRepresentation: ColumnLayout {
        id: mainLayout

        spacing: 2

        readonly property bool isHorizontal: Plasmoid.formFactor === PlasmaCore.Types.Horizontal
        readonly property bool isVertical: Plasmoid.formFactor === PlasmaCore.Types.Vertical

        Layout.fillWidth: isVertical ? true : false
        Layout.fillHeight: isHorizontal ? true : false

        Layout.preferredWidth: {
            var _showSeconds = Plasmoid.configuration.showSeconds; // forces geometry to be recomputed if we change second config
            if (isHorizontal) {
                return Math.max(clockLoader.item ? clockLoader.item.preferredWidthForHeight(clockLoader.item.height) : 0, date.visible ? date.implicitWidth : 0);
            } else
                return parent.width;
        }

        Layout.preferredHeight: {
            var _showSeconds = Plasmoid.configuration.showSeconds;
            if (isVertical) {
                return (clockLoader.item ? clockLoader.item.preferredHeightForWidth(clockLoader.item.width) : 0) + (date.visible ? date.implicitHeight : 0);
            } else
                return parent.height;
        }

        Connections {
            target: Plasmoid.configuration
            function onValueChanged() {
                clockTimer.triggered();

                if (clockLoader.item)
                    clockLoader.item.update();
            }
        }

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
                    text: root.timeString
                }
            }

            Component {
                id: analogComponent
                AnalogClock {
                    id: analogClock
                    Connections {
                        target: root
                        function onTimeStringChanged() {
                            // the clock has absolutely 0 dependance on timeString, however, it's
                            // very useful for determining when we need to do a redraw, instead of
                            // blindly doing on every tick. since timeString already lives within root
                            // it's been loaded in to memory, and there are no penalties there
                            analogClock.update()
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
            text: dateString
        }
    }
}
