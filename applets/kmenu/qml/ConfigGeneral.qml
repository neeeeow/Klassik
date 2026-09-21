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
    property alias cfg_showTitles: showTitles.checked
    property alias cfg_showSearch: showSearch.checked
    property alias cfg_showTooltips: showTooltips.checked

    property alias cfg_showRecentApps: showRecentApps.checked
    property alias cfg_numRecentApps: numRecentApps.value

    property alias cfg_showSettings: showSettings.checked

    property alias cfg_showRecentDocs: showRecentDocs.checked
    property alias cfg_numRecentDocs: numRecentDocs.value

    Kirigami.FormLayout {
        anchors.left: parent.left
        anchors.right: parent.right

        Button {
            id: configIcon

            Kirigami.FormData.label: i18n("Icon:")

            implicitWidth: Kirigami.Units.iconSizes.large + Kirigami.Units.smallSpacing * 2
            implicitHeight: implicitWidth

            icon.name: configGeneral.cfg_icon
            icon.width: Kirigami.Units.iconSizes.large
            icon.height: Kirigami.Units.iconSizes.large

            checkable: true

            KIconThemes.IconDialog {
                id: iconDialog

                onAccepted: {
                    if (iconDialog.iconName != "")
                        configGeneral.cfg_icon = iconDialog.iconName
                }
            }

            onClicked: iconMenu.opened ? iconMenu.close() : iconMenu.open()

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
            Kirigami.FormData.label: i18n("General")
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
        CheckBox {
            id: showSearch
            text: i18n("Display application search bar")
        }
        CheckBox {
            id: showTooltips
            text: i18n("Display tooltips on menu items")
        }

        Item {
            Kirigami.FormData.label: i18n("Menu sections")
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
