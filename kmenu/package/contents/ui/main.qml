import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.plasma.plasmoid
import org.kde.kirigami as Kirigami

PlasmoidItem{
    id: root

    preferredRepresentation: fullRepresentation

    Plasmoid.icon: Plasmoid.configuration.icon

    fullRepresentation: ToolButton { // We can build the menu button in qml, since Qt Quick hands off buttons to Qt.
        id: menuButton

        checkable: true
        checked: Plasmoid.menuActive

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

        onClicked: Plasmoid.toggleMenu(menuButton, root, Plasmoid.location)

        Connections {
            target: Plasmoid
            function onActivated() {
                Plasmoid.toggleMenu(menuButton, root, Plasmoid.location)
            }
        }
    }
}
