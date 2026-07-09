import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid

PlasmoidItem {
    id: root

    width: 200
    height: 200

    preferredRepresentation: fullRepresentation

    fullRepresentation: Item {
        DigitalClock {
            id: clock
            anchors.fill: parent
            anchors.centerIn: parent
        }
    }
}
