import QtQuick
import QtQuick.Controls

import org.kde.iconthemes as KIconThemes
import org.kde.kcmutils as KCMUtils
import org.kde.kirigami as Kirigami
import org.kde.ksvg as KSvg
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid

KCMUtils.SimpleKCM {
    id: configGeneral

    property string cfg_icon: Plasmoid.configuration.icon
    readonly property string cfg_iconDefault: Plasmoid.configuration.iconDefaultValue

    Kirigami.FormLayout {
        anchors.left: parent.left
        anchors.right: parent.right

        Button {
            id: configIcon

            Kirigami.FormData.label: i18n("Icon:")

            implicitWidth: previewFrame.width + Kirigami.Units.smallSpacing * 2
            implicitHeight: previewFrame.height + Kirigami.Units.smallSpacing * 2

            checkable: true

            KIconThemes.IconDialog {
                id: iconDialog

                onAccepted: {
                    if (iconDialog.iconName != "")
                        configGeneral.cfg_icon = iconDialog.iconName
                }
            }

            onClicked: iconMenu.opened ? iconMenu.close() : iconMenu.open()

            KSvg.FrameSvgItem {
                id: previewFrame
                anchors.centerIn: parent
                imagePath: Plasmoid.location === PlasmaCore.Types.Vertical || Plasmoid.location === PlasmaCore.Types.Horizontal
                        ? "widgets/panel-background" : "widgets/background"
                width: Kirigami.Units.iconSizes.large + fixedMargins.left + fixedMargins.right
                height: Kirigami.Units.iconSizes.large + fixedMargins.top + fixedMargins.bottom

                Kirigami.Icon {
                    anchors.centerIn: parent
                    width: Kirigami.Units.iconSizes.large
                    height: width
                    source: configGeneral.cfg_icon
                }
            }

            Menu {
                id: iconMenu

                // Appear below the button
                y: parent.height

                onClosed: configIcon.checked = false;

                MenuItem {
                    text: i18n("Choose icon...")
                    icon.name: "document-open-folder"
                    onClicked: iconDialog.open()
                }
                MenuItem {
                    text: i18n("Reset to default icon")
                    icon.name: "edit-clear"
                    onClicked: configGeneral.cfg_icon = configGeneral.cfg_iconDefault
                }
            }
        }
    }
}
