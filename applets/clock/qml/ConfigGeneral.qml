/*
 *  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.kde.kcmutils as KCMUtils
import org.kde.kirigami as Kirigami
import org.kde.plasma.core as PlasmaCore
import org.kde.plasma.plasmoid
import org.kde.kquickcontrols

KCMUtils.SimpleKCM {
    id: configGeneral

    property alias cfg_useDigitalClock: useDigitalClock.checked
    readonly property bool cfg_useDigitalClockDefault: Plasmoid.configuration.useDigitalClockDefaultValue

    property alias cfg_showDate: showDate.checked
    readonly property bool cfg_showDateDefault: Plasmoid.configuration.showDateDefaultValue
    property alias cfg_showSeconds: showSeconds.checked
    readonly property bool cfg_showSecondsDefault: Plasmoid.configuration.showSecondsDefaultValue
    property alias cfg_showFrame: showFrame.checked
    readonly property bool cfg_showFrameDefault: Plasmoid.configuration.showFrameDefaultValue
    property alias cfg_blinkingDots: blinkingDots.checked
    readonly property bool cfg_blinkingDotsDefault: Plasmoid.configuration.blinkingDotsDefaultValue
    property alias cfg_antialiasing: antialiasing.checked
    readonly property bool cfg_antialiasingDefault: Plasmoid.configuration.antialiasingDefaultValue

    property alias cfg_useSystemColors: useSystemColors.checked
    readonly property bool cfg_useSystemColorsDefault: Plasmoid.configuration.useSystemColorsDefaultValue
    property alias cfg_lcdLook: lcdLook.checked
    readonly property bool cfg_lcdLookDefault: Plasmoid.configuration.lcdLookDefaultValue
    property alias cfg_useCustomColors: useCustomColors.checked
    readonly property bool cfg_useCustomColorsDefault: Plasmoid.configuration.useCustomColorsDefaultValue

    property alias cfg_fgColor: fgColor.color
    readonly property color cfg_fgColorDefault: Plasmoid.configuration.fgColorDefaultValue
    property alias cfg_shadowColor: shadowColor.color
    readonly property color cfg_shadowColorDefault: Plasmoid.configuration.shadowColorDefaultValue
    property alias cfg_bgColor: bgColor.color
    readonly property color cfg_bgColorDefault: Plasmoid.configuration.bgColorDefaultValue

    Kirigami.FormLayout {
        anchors.left: parent.left
        anchors.right: parent.right

        RowLayout {
            RadioButton {
                id: useDigitalClock
                text: i18n("Digital clock")
            }
            RadioButton {
                checked: !useDigitalClock.checked
                text: i18n("Analog clock")
            }
        }

        GroupBox {
            title: i18n("Display")
            RowLayout {
                CheckBox {
                    id: showDate
                    text: i18n("Date")
                }
                CheckBox {
                    id: showSeconds
                    text: i18n("Seconds")
                }
                CheckBox {
                    id: showFrame
                    text: i18n("Frame")
                }
                CheckBox {
                    visible: useDigitalClock.checked
                    id: blinkingDots
                    text: i18n("Blinking dots")
                }
                CheckBox {
                    visible: !useDigitalClock.checked
                    id: antialiasing
                    text: i18n("Antialiasing")
                }
            }
        }

        GroupBox {
            title: i18n("Clock")

            ColumnLayout {

                ColumnLayout {
                    RadioButton {
                        id: useSystemColors
                        text: i18n("Use system colors")
                    }
                    RadioButton {
                        id: lcdLook
                        text: i18n("LCD look")
                    }
                    RadioButton {
                        id: useCustomColors
                        text: i18n("Custom colors")
                    }
                }

                GridLayout {
                    enabled: useCustomColors.checked
                    columns: 2
                    Label {
                        text: i18n("Foreground color:")
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    }
                    ColorButton {
                        id: fgColor
                    }
                    Label {
                        text: i18n("Shadow color:")
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    }
                    ColorButton {
                        id: shadowColor
                    }
                    Label {
                        text: i18n("Background color:")
                        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    }
                    ColorButton {
                        id: bgColor
                    }
                }
            }
        }
    }
}
