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

    property alias cfg_showDate: showDate.checked
    property alias cfg_showSeconds: showSeconds.checked
    property alias cfg_showFrame: showFrame.checked
    property alias cfg_blinkingDots: blinkingDots.checked
    property alias cfg_antialiasing: antialiasing.checked

    property int cfg_colorTheme: 0

    property alias cfg_fgColor: fgColor.color
    property alias cfg_shadowColor: shadowColor.color
    property alias cfg_bgColor: bgColor.color

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
                        text: i18n("Use system colors")
                        checked: cfg_colorTheme === 0
                        onToggled: cfg_colorTheme = 0
                    }
                    RadioButton {
                        text: i18n("LCD look")
                        checked: cfg_colorTheme === 1
                        onToggled: cfg_colorTheme = 1
                    }
                    RadioButton {
                        id: useCustomColors
                        text: i18n("Custom colors")
                        checked: cfg_colorTheme === 2
                        onToggled: cfg_colorTheme = 2
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
