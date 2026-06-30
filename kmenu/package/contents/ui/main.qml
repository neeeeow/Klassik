import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.plasma.plasmoid
import org.kde.kirigami as Kirigami

import com.github.neeeeow.klassik.kmenu as KMenu

PlasmoidItem{
    id: root

    preferredRepresentation: fullRepresentation

    Plasmoid.icon: Plasmoid.configuration.icon

    KMenu.KMenuManager {
        id: kmenu // Create and initialize our KMenu plugin
        Component.onCompleted: kmenu.initialize(Plasmoid.containment)
    }

    fullRepresentation: ToolButton { // We can build the menu button in qml, since Qt Quick hands off buttons to Qt.
        id: menuButton

        // Fill the height of the container
        Layout.fillHeight: true
        Layout.fillWidth: false

        contentItem: Kirigami.Icon {
            height: Math.min(menuButton.height - menuButton.topPadding - menuButton.bottomPadding,
                             menuButton.width - menuButton.leftPadding - menuButton.rightPadding)
            width: height
            anchors.centerIn: parent
            source: Plasmoid.icon
        }

        onClicked: {
            kmenu.showMenu(menuButton, root, Plasmoid.location)
        }
    }
}
