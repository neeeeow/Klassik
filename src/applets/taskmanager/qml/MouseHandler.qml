/*
    SPDX-FileCopyrightText: 2012-2016 Eike Hein <hein@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

import QtQuick

import org.kde.taskmanager as TaskManager
import org.kde.plasma.plasmoid
import plasma.applet.com.github.neeeeow.klassik.taskmanager as TaskManagerApplet

DropArea {
    id: dropArea
    signal urlsDropped(var urls)

    property Item target
    property Item ignoredItem
    property Item hoveredItem
    property bool moved: false

    property alias handleWheelEvents: wheelHandler.handleWheelEvents

    //ignore anything that is neither internal to TaskManager or a URL list
    onEntered: event => {
        if (event.formats.indexOf("text/x-plasmoidservicename") >= 0) {
            event.accepted = false;
        }
        if (target.animating) { // Not all targets have an animating property
            target.animating = false;
        }
    }

    onPositionChanged: event => {
        if (target.animating) {
            return;
        }

        let above;
        above = target.childAt(event.x, event.y);

        if (!above) {
            hoveredItem = null;
            activationTimer.stop();

            return;
        }

        if (tasksModel.sortMode === TaskManager.TasksModel.SortManual && tasks.dragSource) {
            // Reject drags between different TaskList instances.
            if (tasks.dragSource.parent !== above.parent) {
                return;
            }

            const insertAt = above.index;

            if (tasks.dragSource !== above && tasks.dragSource.index !== insertAt) {
                tasksModel.move(tasks.dragSource.index, insertAt);

                ignoredItem = above;
                ignoreItemTimer.restart();
            }
        } else if (!tasks.dragSource && hoveredItem !== above) {
            hoveredItem = above;
            activationTimer.restart();
        }
    }

    onExited: {
        hoveredItem = null;
        activationTimer.stop();
    }

    onDropped: event => {
        // Reject internal drops.
        if (event.formats.indexOf("application/x-orgkdeplasmataskmanager_taskbuttonitem") >= 0) {
            event.accepted = false;
            return;
        }

        // Reject plasmoid drops.
        if (event.formats.indexOf("text/x-plasmoidservicename") >= 0) {
            event.accepted = false;
            return;
        }

        if (event.hasUrls) {
            urlsDropped(event.urls);
            return;
        }
    }

    Connections {
        target: tasks

        function onDragSourceChanged(): void {
            if (!dragSource) {
                dropArea.ignoredItem = null;
                ignoreItemTimer.stop();
            }
        }
    }

    Timer {
        id: ignoreItemTimer

        repeat: false
        interval: 750

        onTriggered: {
            dropArea.ignoredItem = null;
        }
    }

    Timer {
        id: activationTimer

        interval: 250
        repeat: false

        onTriggered: {
            if (parent.hoveredItem.model.IsGroupParent) {
                parent.hoveredItem.model.showGroupMenu();
            } else {
                tasksModel.requestActivate(parent.hoveredItem.modelIndex());
            }
        }
    }

    WheelHandler {
        id: wheelHandler

        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad

        property bool handleWheelEvents: true

        enabled: handleWheelEvents && Plasmoid.configuration.wheelEnabled !== 0

        onWheel: event => {
            // magic number 15 for common "one scroll"
            // See https://doc.qt.io/qt-6/qml-qtquick-wheelhandler.html#rotation-prop
            let increment = 0;
            while (rotation >= 15) {
                rotation -= 15;
                increment++;
            }
            while (rotation <= -15) {
                rotation += 15;
                increment--;
            }
            const anchor = dropArea.target.childAt(event.x, event.y);
            while (increment !== 0) {
                TaskManagerApplet.TaskTools.activateNextPrevTask(anchor, increment < 0, Plasmoid.configuration.wheelSkipMinimized, Plasmoid.configuration.wheelEnabled, tasks);
                increment += (increment < 0) ? 1 : -1;
            }
        }
    }
}
