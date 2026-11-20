import QtQuick
import QtQuick.Controls

ToolButton {
    id: button

    background: ToolButtonBackground {
        hovered: button.hovered
        sunken: button.down || button.checked
        autoRaise: button.flat
    }
}
