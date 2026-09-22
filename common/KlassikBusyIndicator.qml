/*
   SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

   SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick

AnimatedSprite {
    id: root

    visible: running // Required to mimic a busy indicator

    implicitWidth: 16
    implicitHeight: 16

    source: "busyindicator.png"
    frameWidth: 16
    frameHeight: 16
    frameCount: 10
    frameDuration: 100

    onRunningChanged: {
        if (!running) {
            currentFrame = 0
        }
    }
}
