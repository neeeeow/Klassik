/*
 *  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>
 *
 *  SPDX-FileCopyrightText: 2012-2013 Eike Hein <hein@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

.import org.kde.kirigami as Kirigami

function preferredMaxWidth() {
    return [150, 200, 250][tasks.plasmoid.configuration.taskMaxWidth] ?? 200;
}

function preferredMaxHeight() {
    return [18, 26, 34][tasks.plasmoid.configuration.taskMaxHeight] ?? 18;
}

function stripeCount() {
    if (tasks.vertical) {
        return 1;
    } else {
        const maxHeight = preferredMaxHeight();
        return Math.max(1, Math.floor(tasks.taskAreaHeight / maxHeight)); // KDE 3 logic
    }
}

function orthogonalCount(count) {
    if (count <= 0)
        return 1;
    const stripes = stripeCount();
    return Math.ceil(count / stripes);
}

function optimumCapacity(width, height) {
    const length = tasks.vertical ? height : width;
    const maximum = tasks.vertical ? preferredMaxHeight() : preferredMaxWidth();

    if (!tasks.vertical) {
        //  Fit more tasks in this case, that is possible to cut text, before combining tasks.
        return Math.ceil(length / maximum) * stripeCount() + 1;
    }

    return Math.floor(length / maximum) * stripeCount();
}

function spaceRequiredToShowText() {
    // gridUnit is the height of the default font, but only one isn't enough to
    // show anything but the elision character. 2 is too high and results in
    // text appearing only at excessively high widths.
    return Math.round(Kirigami.Units.gridUnit * 1.5);
}
