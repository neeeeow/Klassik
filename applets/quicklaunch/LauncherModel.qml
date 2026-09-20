/*
 *  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>
 *
 *  SPDX-FileCopyrightText: 2015 David Rosca <nowrep@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick

ListModel {
    // NOTE: here, id refers to storageId, but I'd rather not write storageId
    // a million times.
    id: listModel

    property int dropMarkerIndex : -1

    signal idsChanged()

    function ids()
    {
        var out = [];
        for (var i = 0; i < listModel.count; ++i) {
            out.push(get(i).storageId);
        }
        return out;
    }

    function setIds(ids)
    {
        clear();
        insertIds(0, ids);

        idsChanged();
    }

    function appendId(storageId)
    {
        append({ storageId: storageId });

        idsChanged();
    }

    function insertId(index, storageId)
    {
        insert(index, { storageId: storageId });

        idsChanged();
    }

    function insertIds(index, ids)
    {
        for (var i = 0; i < ids.length; ++i) {
            insert(index + i, { storageId: ids[i] });
        }

        if (ids.length) {
            idsChanged();
        }
    }

    function moveId(from, to)
    {
        if (from == -1 || to == -1 || from == to) {
            return false;
        }

        move(from, to, 1);

        idsChanged();
        return true;
    }

    function removeId(index)
    {
        remove(index, 1);

        idsChanged();
    }

    // Drop marker is internally represented as a "quicklaunch:drop" id
    function showDropMarker(index)
    {
        if (index == -1) {
            index = dropMarkerIndex == -1 ? count : count - 1;
        }

        if (dropMarkerIndex != -1) {
            move(dropMarkerIndex, index, 1);
            dropMarkerIndex = index;
        } else {
            insert(index, { storageId: "quicklaunch:drop" });
            dropMarkerIndex = index;
        }
    }

    function clearDropMarker()
    {
        if (dropMarkerIndex != -1) {
            remove(dropMarkerIndex, 1);
            dropMarkerIndex = -1;
        }
    }
}
