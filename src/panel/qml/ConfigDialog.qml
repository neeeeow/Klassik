/*
    SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
 */
pragma ComponentBehavior: Bound

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

    property url customBackgroundUrl

    function save(): void {
        if (Plasmoid.immutable)
            // Don't save if the panel is immutable!
            return;

        Plasmoid.configuration.drawFrame = drawFrame.checked;
        Plasmoid.configuration.showAppletHandle = showAppletHandle.checked;
        Plasmoid.configuration.colorize = colorize.checked;
        Plasmoid.configuration.useBackground = useBackground.checked;
        Plasmoid.configuration.useCustomBackground = useCustomBackground.checked;
        Plasmoid.configuration.customBackgroundUrl = root.customBackgroundUrl;
        Plasmoid.configuration.writeConfig();
    }

    onVisibleChanged: {
        if (visible) {
            drawFrame.checked = Plasmoid.configuration.drawFrame;
            showAppletHandle.checked = Plasmoid.configuration.showAppletHandle;
            colorize.checked = Plasmoid.configuration.colorize;
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
                nameFilters: [i18n("Image files (*.bmp *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.xbm *.xpm *.svg)")]
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
                id: colorize
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
                    text: i18n("Custom background…")
                }
            }

            RowLayout {
                enabled: useBackground.checked && useCustomBackground.checked
                Label {
                    text: i18n("Set custom panel background:")
                }

                Button {
                    onClicked: backgroundDialog.open()
                    Layout.preferredWidth: 48
                    Layout.preferredHeight: 48
                    Layout.maximumWidth: 48
                    Layout.maximumHeight: 48

                    contentItem: Image {
                        source: root.customBackgroundUrl
                        fillMode: Image.PreserveAspectFit
                    }
                }

                TextField {
                    readOnly: true
                    Layout.fillWidth: true
                    text: root.customBackgroundUrl.toString()
                    placeholderText: i18n("No image selected…")
                }
            }
        }

        DialogButtonBox {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignBottom
            standardButtons: DialogButtonBox.Ok | DialogButtonBox.Apply | DialogButtonBox.Cancel
            Component.onCompleted: {
                // Disable the OK/Apply buttons if the panel is immutable (thus preventing saving)
                standardButton(DialogButtonBox.Ok).enabled = Qt.binding(() => !Plasmoid.immutable)
                standardButton(DialogButtonBox.Apply).enabled = Qt.binding(() => !Plasmoid.immutable)
            }

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
