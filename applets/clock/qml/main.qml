import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid

PlasmoidItem {
    id: root

    width: 200
    height: 200

    property string timeString: ""
    Timer { // Internal timer used for keeping track of our clock
        id: clockTimer
        interval: 500
        running: true
        repeat: true
        triggeredOnStart: true

        property bool showDots: true

        onTriggered: {
            var time = new Date()

            clockTimer.showDots = !clockTimer.showDots
            var hourString = Qt.formatTime(time, "hh")
            var minuteString = Qt.formatTime(time, "mm")
            var secondString = Qt.formatTime(time, "ss")
            var separator = clockTimer.showDots ? ":" : " "
            var timeString = hourString + separator + minuteString + separator + secondString

            root.timeString = timeString
        }
    }

    preferredRepresentation: fullRepresentation
    fullRepresentation: Item {
        DigitalClock {
            id: clock
            anchors.fill: parent
            anchors.centerIn: parent

            text: root.timeString
        }
    }
}
