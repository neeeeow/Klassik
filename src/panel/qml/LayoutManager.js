/*
    SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>

    SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2022 Niccolò Venerandi <niccolo@venerandi.com>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

var layout;
var root;
var appletsModel;

function addApplet(applet, x, y) {
    let appletItem = root.itemFor(applet);
    if (!appletItem)
        return;
    const new_element = {applet: appletItem};

    if (x >= 0 && y >= 0) {
        appletsModel.insert(indexAtCoordinates(x, y), new_element)
    } else {
        appletsModel.append(new_element);
    }
}

function restore() {
    var configString = String(root.Plasmoid.configuration.AppletOrder)

    //array, a cell for encoded item order
    var itemsArray = configString.split(";");

    //map applet id->order in panel
    var idsOrder = new Object();
    //map order in panel -> applet pointer
    var appletsOrder = new Object();

    for (var i = 0; i < itemsArray.length; i++) {
        //property name: applet id
        //property value: order
        idsOrder[itemsArray[i]] = i;
    }

    for (var i = 0; i < root.Containment.applets.length; ++i) {
        if (idsOrder[root.Containment.applets[i].id] !== undefined) {
            appletsOrder[idsOrder[root.Containment.applets[i].id]] = root.Containment.applets[i];
        //ones that weren't saved in AppletOrder go to the end
        } else {
            appletsOrder["unordered"+i] = root.Containment.applets[i];
        }
    }

    //finally, restore the applets in the correct order
    for (var i in appletsOrder) {
        addApplet(appletsOrder[i], -1, -1)
    }
    //rewrite, so if in the orders there were now invalid ids or if some were missing creates a correct list instead
    save();
}

function save() {
    const ids = layout.children
        .map(child => child?.applet?.Plasmoid?.id)
        .filter(id => id !== undefined)

    root.Plasmoid.configuration.AppletOrder = ids.join(';');
    root.Plasmoid.configuration.writeConfig();
}

function childAtCoordinates(x, y) {
    if (root.isHorizontal) {
        y = layout.height / 2;
    } else {
        x = layout.width / 2;
    }
    /*
     * When adding a new panel, childAt will return lastSpacer, and that's where
     * `index` property works.
     */
    var child = layout.childAt(x, y);
    while (!child) {
        if (root.isHorizontal) {
            // Only yields incorrect results for widgets smaller than the
            // row/column spacing, which is luckily fairly unrealistic
            x -= Math.max(1, layout.columnSpacing); // now that we set spacing to 0, add this to ensure x,y do get decremented
        } else {
            y -= Math.max(1, layout.rowSpacing);
        }
        if (x < 0 || y < 0) {
            return null;
        }
        child = layout.childAt(x, y);
    }
    return child
}

function indexAtCoordinates(x, y) {
    const child = childAtCoordinates(x, y);
    if (!child)
        return 0;
    const idx = ((!root.isHorizontal && y < child.y + child.height / 2) ||
    (root.isHorizontal && x < child.x + child.width / 2))
                ? child.index
                : child.index + 1;
    return Math.min(idx, appletsModel.count);
}

function move(applet, end) {
    const start = applet.index;
    const target = Math.max(0, Math.min(end - (start < end ? 1 : 0), appletsModel.count - 1));
    if (start === target)
        return;
    appletsModel.move(start, target, 1);
}
