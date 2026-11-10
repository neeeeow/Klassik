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
    property alias cfg_showGenericName: showGenericName.checked

    property alias cfg_showRecentApps: showRecentApps.checked
    property int cfg_recentAppsType: 0
    property alias cfg_numRecentApps: numRecentApps.value

    property alias cfg_showRecentDocs: showRecentDocs.checked
    property alias cfg_numRecentDocs: numRecentDocs.value

    property alias cfg_showSystem: showSystem.checked
    property alias cfg_showSettings: showSettings.checked

    Kirigami.FormLayout {
        anchors.left: parent.left
        anchors.right: parent.right

        Button {
            id: configIcon

            Kirigami.FormData.label: i18n('Icon:')

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
                    text: i18n('Choose icon…')
                    icon.name: 'document-open-folder'
                    onClicked: iconDialog.open()
                }
                MenuItem {
                    text: i18n('Reset to default icon')
                    icon.name: 'edit-clear'
                    onClicked: configGeneral.cfg_icon = configGeneral.cfg_iconDefault
                }
            }
        }

        Item {
            Kirigami.FormData.isSection: true
        }

        GroupBox {
            title: i18n("K Menu Layout")
            GridLayout {
                columns: 2

                CheckBox {
                    id: drawSideImage
                    text: i18n('Show side image')
                }
                CheckBox {
                    id: showTitles
                    text: i18n('Show section titles')
                }
                CheckBox {
                    id: showSearch
                    text: i18n('Show applications search bar')
                }
                CheckBox {
                    id: showRecentApps
                    text: i18n('Show "Quick Start" section')
                }
                CheckBox {
                    id: showRecentDocs
                    text: i18n('Show "Recent Documents" submenu')
                }
                CheckBox {
                    id: showSystem
                    text: i18n('Show "My System" submenu')
                }
                CheckBox {
                    id: showSettings
                    text: i18n('Show "Settings" submenu')
                }
                CheckBox {
                    id: showTooltips
                    text: i18n('Display tooltips on menu items')
                }
                CheckBox {
                    id: showGenericName
                    text: i18n('Detailed menu entries')
                }
            }
        }

        Item {
            Kirigami.FormData.isSection: true
        }

        RowLayout {
            enabled: showRecentApps.checked
            spacing: Kirigami.Units.smallSpacing

            Label {
                text: i18n('Max number of "Quick Start" entries:')
            }

            SpinBox {
                id: numRecentApps
                from: 1
                to: 20
            }
        }

        GroupBox {
            enabled: showRecentApps.checked
            title: i18n('"Quick Start" section contains')
            RowLayout {
                RadioButton {
                    text: i18n('Most recently used items')
                    checked: cfg_recentAppsType === 0
                    onToggled: cfg_recentAppsType = 0
                }
                RadioButton {
                    text: i18n('Most frequently used items')
                    checked: cfg_recentAppsType === 1
                    onToggled: cfg_recentAppsType = 1
                }
            }
        }

        Item {
            Kirigami.FormData.isSection: true
        }

        RowLayout {
            enabled: showRecentDocs.checked
            spacing: Kirigami.Units.smallSpacing

            Label {
                text: i18n('Max number of "Recent Documents" entries:')
            }

            SpinBox {
                id: numRecentDocs
                from: 1
                to: 20
            }
        }
    }
}
