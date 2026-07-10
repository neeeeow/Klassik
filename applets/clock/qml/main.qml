import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid

PlasmoidItem {
    id: root

    property string timeString: "12:34" // use a default value just in case
    property string dateString: ""
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
            var timeString = hourString + separator + minuteString

            root.timeString = timeString
            root.dateString = Qt.formatDate(time, "dd/MM/yyyy")
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
            if (isHorizontal) {
                return Math.max(clock.preferredWidthForHeight(clock.height), date.width);
            } else
                return parent.width;
        }

        Layout.preferredHeight: {
            if (isVertical) {
                return clock.preferredHeightForWidth(clock.width) + (date.visible ? date.height : 0);
            } else
                return parent.height;
        }

        DigitalClock {
            id: clock
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            Layout.fillHeight: true

            text: root.timeString
        }

        Label {
            id: date
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 8
            text: dateString
        }
    }
}
