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

        contentItem: Kirigami.Icon {
            source: Plasmoid.icon
            active: menuButton.hovered

            scale: (menuButton.down || menuButton.checked) ? (width - 2) / width : 1.0
            transformOrigin: Item.Center
        }

        PanelButtonArrow {
            anchors.fill: parent
            active: menuButton.down || menuButton.checked
            location: {
                switch (Plasmoid.location) {
                    case PlasmaCore.Types.TopEdge:
                        return PanelButtonArrow.Below;
                    case PlasmaCore.Types.LeftEdge:
                        return PanelButtonArrow.Right;
                    case PlasmaCore.Types.RightEdge:
                        return PanelButtonArrow.Left;
                    default:
                        return PanelButtonArrow.Above;
                }
            }
        }

        onClicked: Plasmoid.toggleMenu(menuButton, Plasmoid.location)
        Connections {
            target: Plasmoid
            function onActivated() {
                Plasmoid.toggleMenu(menuButton, Plasmoid.location)
            }
        }
    }
}
