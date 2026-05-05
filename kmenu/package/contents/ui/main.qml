import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.plasma.plasmoid
//import org.kde.plasma.components as PlasmaComponents // try to avoid using if possible

import com.github.neeeeow.klassik.kmenu as KMenu

PlasmoidItem{
    id: root

    preferredRepresentation: fullRepresentation

    KMenu.KMenuManager {
        id: kmenu // Create and initialize our KMenu plugin
    }

    fullRepresentation: ToolButton { // We can build the menu button in qml, since Qt Quick hands off buttons to Qt.
        id: menuButton
        text: "Click Me"

        // Fill the height of the container
        Layout.fillHeight: true
        Layout.fillWidth: false
        Layout.preferredWidth: height // Set width equal to height

        onClicked: {
            kmenu.showMenu(menuButton, root, Plasmoid.location)
        }

    }
}
