/*
 *  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

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

    property alias cfg_drawSideImage: drawSideImage.checked
    readonly property bool cfg_drawSideImageDefault: Plasmoid.configuration.drawSideImageDefaultValue // Matches main.xml default

    property alias cfg_showTitles: showTitles.checked
    readonly property bool cfg_showTitlesDefault: Plasmoid.configuration.showTitlesDefaultValue

    property alias cfg_showRecentApps: showRecentApps.checked
    readonly property bool cfg_showRecentAppsDefault: Plasmoid.configuration.showRecentAppsDefaultValue
    property alias cfg_numRecentApps: numRecentApps.value
    readonly property int cfg_numRecentAppsDefault: Plasmoid.configuration.numRecentAppsDefaultValue

    property alias cfg_showSearch: showSearch.checked
    readonly property bool cfg_showSearchDefault: Plasmoid.configuration.showSearchDefaultValue

    property alias cfg_showSettings: showSettings.checked
    readonly property bool cfg_showSettingsDefault: Plasmoid.configuration.showSettingsDefaultValue

    property alias cfg_showRecentDocs: showRecentDocs.checked
    readonly property bool cfg_showRecentDocsDefault: Plasmoid.configuration.showRecentDocsDefaultValue
    property alias cfg_numRecentDocs: numRecentDocs.value
    readonly property int cfg_numRecentDocsDefault: Plasmoid.configuration.numRecentDocsDefaultValue

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

        Item {
            Kirigami.FormData.label: i18n("General:")
            Kirigami.FormData.isSection: true
        }

        CheckBox {
            id: drawSideImage
            text: i18n("Display side image")
        }
        CheckBox {
            id: showTitles
            text: i18n("Display section titles")
        }

        Item {
            Kirigami.FormData.isSection: true
        }

        CheckBox {
            id: showRecentApps
            text: i18n("Display most used applications")
        }
        RowLayout {
            enabled: showRecentApps.checked
            spacing: Kirigami.Units.smallSpacing

            Label {
                text: i18n("Number of applications:")
            }

            SpinBox {
                id: numRecentApps
                from: 1
                to: 20
            }
        }

        Item {
            Kirigami.FormData.isSection: true
        }

        CheckBox {
            id: showSearch
            text: i18n("Display application search bar")
        }
        CheckBox {
            id: showSettings
            text: i18n("Display settings submenu")
        }

        Item {
            Kirigami.FormData.isSection: true
        }

        CheckBox {
            id: showRecentDocs
            text: i18n("Display recent documents")
        }
        RowLayout {
            enabled: showRecentDocs.checked
            spacing: Kirigami.Units.smallSpacing

            Label {
                text: i18n("Number of documents:")
            }

            SpinBox {
                id: numRecentDocs
                from: 1
                to: 20
            }
        }
    }
}
