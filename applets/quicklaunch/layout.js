/*
 *  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

.import org.kde.plasma.core as PlasmaCore
.import org.kde.kirigami as Kirigami

function preferredExtent()
{
    return preferredCellExtent() * Math.ceil(grid.count / sectionCount);
}

function preferredCellExtent()
{
    return Math.floor((horizontal ? root.height : root.width) / sectionCount);
}
