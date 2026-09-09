/*
    SPDX-FileCopyrightText: 2012-2013 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

.import org.kde.kirigami as Kirigami

const iconMargin = Math.round(Kirigami.Units.smallSpacing / 4);
const labelMargin = Kirigami.Units.smallSpacing;

function preferredMaxWidth() {
    let maxWidth = 200;
    switch (tasks.plasmoid.configuration.taskMaxWidth) {
        case 0: // narrow
            maxWidth = 150;
            break;
        case 1: // medium
            maxWidth = 200;
            break;
        case 2: // wide
            maxWidth = 250;
            break;
    }

    return maxWidth;
}

function preferredMaxHeight() {
    return 26; // TODO: implement config for this
}

function stripeCount() {
    if (tasks.vertical) {
        return 1;
    } else {
        const maxHeight = preferredMaxHeight();
        return Math.max(1, Math.floor(tasks.height / maxHeight)); // KDE 3 logic
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

function maximumContextMenuTextWidth() {
    return (Kirigami.Units.iconSizes.sizeForLabels * 28);
}
