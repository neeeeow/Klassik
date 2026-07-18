/*
 *  SPDX-FileCopyrightText: 2026 neeeeow <https://github.com/neeeeow>
 *
 *  SPDX-FileCopyrightText: 2015 David Rosca <nowrep@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

.import org.kde.plasma.core as PlasmaCore
.import org.kde.kirigami as Kirigami

function itemPadding() { return 0; }

function rows()
{
    if (vertical) {
        return Math.ceil(grid.count / maxSectionCount);
    }
    return Math.min(grid.count, maxSectionCount);
}

function cols()
{
    if (vertical) {
        return Math.min(grid.count, maxSectionCount);
    }
    return Math.ceil(grid.count / maxSectionCount);
}

function minimumWidth()
{
    return cols() * minimumCellWidth();
}

function minimumHeight()
{
    var h = rows() * minimumCellHeight();
    if (title.length) {
        h += titleLabel.height;
    }
    return h;
}

function preferredWidth()
{
    var w = cols() * preferredCellWidth();
    // may be briefly horizontal and vertical when switching
    // orientation due to binding order. Avoid infinite regression,
    // we get the correct value when the bindings settle
    if (horizontal && !vertical) {
        w = (preferredHeight() / rows()) * cols();
    }
    return w;
}

function preferredHeight()
{
    var h = rows() * preferredCellHeight();
    if (vertical && !horizontal) {
        h = (preferredWidth() / cols()) * rows();
    }
    if (title.length) {
        h += titleLabel.height;
    }
    return h;
}

function minimumCellWidth()
{
  return Kirigami.Units.iconSizes.small + 2 * itemPadding();
}

function minimumCellHeight()
{
  var h = Kirigami.Units.iconSizes.small + 2 * itemPadding();
  if (showLauncherNames) {
    h += Kirigami.Units.gridUnit * 2;
  }
    return h;
}

function preferredCellWidth()
{
    return Math.floor(grid.width / cols());
}

function preferredCellHeight()
{
    return Math.floor(grid.height / rows());
}
