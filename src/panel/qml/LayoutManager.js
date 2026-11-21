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
            x -= Math.max(1, layout.rowSpacing); // now that we set spacing to 0, add this to ensure x,y do get decremented
        } else {
            y -= Math.max(1, layout.columnSpacing);
        }
        if (x < 0 || y < 0) {
            return 0;
        }
        child = layout.childAt(x, y);
    }
    return child
}

function indexAtCoordinates(x, y) {
    let child = childAtCoordinates(x, y)
    if ((!root.isHorizontal && y < child.y + child.height/2) ||
        (root.isHorizontal && x < child.x + child.width/2)) {
        return child.index;
    } else {
        return child.index+1;
    }
}

function move(applet, end) {
    var start = applet.index
    var target = end - (start < end)
    if (start == target) return;
    appletsModel.move(start, target, 1)
}
