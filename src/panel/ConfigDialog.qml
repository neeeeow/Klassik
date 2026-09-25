/*
 *  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs
import org.kde.plasma.plasmoid
import org.kde.kirigami as Kirigami

Window {
    id: root
    title: i18n("Klassik Panel Preferences")
    flags: Qt.Dialog
    color: Kirigami.Theme.backgroundColor

    minimumWidth: Kirigami.Units.gridUnit * 28
    minimumHeight: Kirigami.Units.gridUnit * 22

    property var customBackgroundUrl

    function save() {
        Plasmoid.configuration.drawFrame = drawFrame.checked;
        Plasmoid.configuration.showAppletHandle = showAppletHandle.checked;
        Plasmoid.configuration.colorizePanel = colorizePanel.checked;
        Plasmoid.configuration.useBackground = useBackground.checked;
        Plasmoid.configuration.useCustomBackground = useCustomBackground.checked;
        Plasmoid.configuration.customBackgroundUrl = root.customBackgroundUrl;
    }

    onVisibleChanged: {
        if (visible) {
            drawFrame.checked = Plasmoid.configuration.drawFrame;
            showAppletHandle.checked = Plasmoid.configuration.showAppletHandle;
            colorizePanel.checked = Plasmoid.configuration.colorizePanel;
            useBackground.checked = Plasmoid.configuration.useBackground;
            useCustomBackground.checked = Plasmoid.configuration.useCustomBackground;
            root.customBackgroundUrl = Plasmoid.configuration.customBackgroundUrl;
        }
    }

    ColumnLayout {
        anchors.fill: parent
        Kirigami.FormLayout {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.fillHeight: true

            FileDialog {
                id: backgroundDialog
                title: i18n("Select panel background")
                nameFilters: ["Image files (*.bmp *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.xbm *.xpm *.svg)"]
                fileMode: FileDialog.OpenFile

                onAccepted: {
                    root.customBackgroundUrl = backgroundDialog.selectedFile
                }
            }

            Item {
                Kirigami.FormData.label: i18n("General")
                Kirigami.FormData.isSection: true
            }

            CheckBox {
                id: drawFrame
                text: i18n("Draw a frame around the panel")
            }
            CheckBox {
                id: showAppletHandle
                text: i18n("Always display applet handle")
            }
            CheckBox {
                id: colorizePanel
                text: i18n("Colorize the panel to match the desktop color scheme")
            }
            CheckBox {
                id: useBackground
                text: i18n("Enable background image")
            }

            ColumnLayout {
                enabled: useBackground.checked
                RadioButton {
                    checked: !useCustomBackground.checked
                    text: i18n("Use default background")
                }
                RadioButton {
                    id: useCustomBackground
                    text: i18n("Custom background...")
                }
            }

            RowLayout {
                enabled: useBackground.checked && useCustomBackground.checked
                Label {
                    text: "Set custom panel background:"
                }

                Button {
                    id: setBackground
                    onClicked: backgroundDialog.open()
                    Layout.preferredWidth: 48
                    Layout.preferredHeight: 48
                    Layout.maximumWidth: 48
                    Layout.maximumHeight: 48

                    contentItem: Image {
                        id: backgroundPreview
                        source: root.customBackgroundUrl
                        fillMode: Image.PreserveAspectFit
                    }
                }

                TextField {
                    id: imageUrl
                    readOnly: true
                    Layout.fillWidth: true
                    text: root.customBackgroundUrl.toString()
                    placeholderText: i18n("No image selected...")
                }
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignBottom

            Text {
                text: "Klassik " + Plasmoid.metaData.version
                font.italic: true
                font.bold: true
                font.pointSize: 16
                opacity: 0.3
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                Layout.leftMargin: Kirigami.Units.largeSpacing
            }

            DialogButtonBox {
                Layout.alignment: Qt.AlignRight
                standardButtons: DialogButtonBox.Ok | DialogButtonBox.Apply | DialogButtonBox.Cancel

                onAccepted: {
                    root.save()
                    root.close()
                }
                onApplied: {
                    root.save()
                }
                onRejected: {
                    root.close()
                }
            }
        }
    }
}
