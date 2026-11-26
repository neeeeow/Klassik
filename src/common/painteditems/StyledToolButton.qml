/*
    SPDX-FileCopyrightText: 2026-2027 neeeeow <https://github.com/neeeeow>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
pragma ComponentBehavior: Bound

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
