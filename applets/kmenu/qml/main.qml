import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.plasma.plasmoid
import org.kde.kirigami as Kirigami

PlasmoidItem{
    id: root

    preferredRepresentation: fullRepresentation

    Plasmoid.icon: Plasmoid.configuration.icon

    fullRepresentation: Button { // We can build the menu button in qml, since Qt Quick hands off buttons to Qt.
        id: menuButton

        flat: true
        checkable: true
        checked: Plasmoid.menuActive

        // Fill the height of the container
        Layout.fillWidth: isVertical ? true : false
        Layout.fillHeight: isHorizontal ? true : false

        background: PanelButtonBackground {
            visible: menuButton.down || menuButton.checked
        }

        contentItem: Kirigami.Icon {
            readonly property int maxIconSize: Math.min(
                menuButton.height - menuButton.topPadding - menuButton.bottomPadding,
                menuButton.width - menuButton.leftPadding - menuButton.rightPadding
            )

            height: maxIconSize
            width: maxIconSize

            anchors.centerIn: parent
            source: Plasmoid.icon
            active: menuButton.hovered

            scale: (menuButton.down || menuButton.checked)
            ? (maxIconSize - 2) / maxIconSize : 1.0
            transformOrigin: Item.Center
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
