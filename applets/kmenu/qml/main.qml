import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import org.kde.plasma.plasmoid
import org.kde.plasma.core as PlasmaCore
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
        readonly property bool isHorizontal: Plasmoid.formFactor === PlasmaCore.Types.Horizontal
        readonly property bool isVertical: Plasmoid.formFactor === PlasmaCore.Types.Vertical
        Layout.fillWidth: isVertical ? true : false
        Layout.fillHeight: isHorizontal ? true : false

        background: PanelButtonBackground {
            visible: menuButton.down || menuButton.checked
        }

        contentItem: Item {
            id: container
            readonly property int maxIconSize: Math.min(
                menuButton.height - menuButton.topPadding - menuButton.bottomPadding,
                menuButton.width - menuButton.leftPadding - menuButton.rightPadding
            )
            height: maxIconSize
            width: maxIconSize
            anchors.centerIn: parent

            Kirigami.Icon {
                anchors.fill: parent
                source: Plasmoid.icon
                active: menuButton.hovered

                scale: (menuButton.down || menuButton.checked)
                ? (container.maxIconSize - 2) / container.maxIconSize : 1.0
                transformOrigin: Item.Center
            }

            PanelButtonArrow {
                anchors.fill: parent
                active: menuButton.down || menuButton.checked
            }
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
