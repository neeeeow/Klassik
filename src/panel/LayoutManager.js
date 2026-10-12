/*
    SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2022 Niccolò Venerandi <niccolo@venerandi.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

var layout;
var root;
var appletsModel;

function addApplet(applet, x, y) {
    let appletItem = root.itemFor(applet);
    // don't show applet if it chooses to be hidden but still make it
    // accessible in the panelcontroller
    // Due to the nature of how "visible" propagates in QML, we need to
    // explicitly set it on the container (so the Layout ignores it)
    // as well as the applet (so it reliably knows about), otherwise it can
    // happen that an applet erroneously thinks it's visible, or suddenly
    // starts thinking that way on teardown (virtual desktop pager)
    // leading to crashes
    const new_element = {applet: appletItem};

    appletItem.visible = Qt.binding(function() {
        return applet.status !== PlasmaCore.Types.HiddenStatus || (!root.plasmoid.immutable && root.plasmoid.userConfiguring) || root.plasmoid.corona.editMode;
    });

    if (x >= 0 && y >= 0) {
        appletsModel.insert(indexAtCoordinates(x, y), new_element)
    } else {
        appletsModel.append(new_element);
    }
}

function restore() {
    var configString = String(root.plasmoid.configuration.AppletOrder)

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

    for (var i = 0; i < root.plasmoid.applets.length; ++i) {
        if (idsOrder[root.plasmoid.applets[i].id] !== undefined) {
            appletsOrder[idsOrder[root.plasmoid.applets[i].id]] = root.plasmoid.applets[i];
        //ones that weren't saved in AppletOrder go to the end
        } else {
            appletsOrder["unordered"+i] = root.plasmoid.applets[i];
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
        .map(child => child?.applet?.plasmoid?.id)
        .filter(id => id !== undefined)

    root.plasmoid.configuration.AppletOrder = ids.join(';');
    root.plasmoid.configuration.writeConfig();
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
    if ((root.plasmoid.formFactor === 3 && y < child.y + child.height/2) ||
        (root.plasmoid.formFactor !== 3 && x < child.x + child.width/2)) {
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
