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

    property alias cfg_showDate: showDate.checked
    readonly property bool cfg_showDateDefault: Plasmoid.configuration.showDateDefaultValue
    property alias cfg_showSeconds: showSeconds.checked
    readonly property bool cfg_showSecondsDefault: Plasmoid.configuration.showSecondsDefaultValue
    property alias cfg_showFrame: showFrame.checked
    readonly property bool cfg_showFrameDefault: Plasmoid.configuration.showFrameDefaultValue
    property alias cfg_blinkingDots: blinkingDots.checked
    readonly property bool cfg_blinkingDotsDefault: Plasmoid.configuration.blinkingDotsDefaultValue

    Kirigami.FormLayout {
        anchors.left: parent.left
        anchors.right: parent.right

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
                id: blinkingDots
                text: i18n("Blinking dots")
            }
        }
    }
}
