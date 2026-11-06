# KDE Modern Classic

A modern take on the KDE 2/3 "KDE Classic" icons: a complete SVG icon theme for KDE Plasma 6.

It is not a one-to-one copy of the old bitmaps. The classic designs are redrawn as clean vectors,
some are reworked for today's desktop, and everything KDE 3 never had (weather, modern tray
states, Wayland and other newer settings) is drawn new in the same style.

- 3,182 icon names, drawn as 1,237 hand-made SVG icons
- Covers Plasma's desktop and panel, the system tray, Dolphin, System Settings, Discover and
  the main KDE apps, so none of them fall back to Breeze
- Crisp at every size, with 536 versions tuned for 16 and 22 px (folders, file types, menus)
- The classic look: glossy colours, top-left lighting, dark outlines, KDE 3's teal arrows

This is a work in progress. Feedback and contributions are welcome:
https://github.com/nzoomed/kde-modern-classic

Dolphin is shown with KDE Classic's filing cabinet, since Dolphin came after KDE 3. If you'd rather see
a dolphin, install the small add-on theme "KDE Modern Classic (Dolphin)" as well and choose it instead.

## Install

System Settings > Colours & Themes > Icons > Install from File, and pick this archive.

Or by hand:

    mkdir -p ~/.local/share/icons
    tar -xzf KDE-Modern-Classic.tar.gz -C ~/.local/share/icons

then choose "KDE Modern Classic" under System Settings > Colours & Themes > Icons.

When updating, remove the old version first, then install the new one. Installing on top of an
older copy can leave some old icons in place:

    rm -rf ~/.local/share/icons/KDE-Modern-Classic
    rm -f ~/.cache/icon-cache.kcache

then install as above and log out and back in.

The theme inherits from `kdeclassic`, then `breeze` and `hicolor`. Plasma itself needs nothing
else; for icons belonging to other apps, installing the KDE Classic bitmap theme as well gives
classic-style fallbacks instead of Breeze ones.

Your Plasma Style can override tray icons (Breeze's Plasma Style does). To see this theme's tray
icons, use a Plasma Style that doesn't ship its own.

## Found a Breeze icon?

`check-app-icons.sh` (included) lists every app in your menu whose icon doesn't come from this theme, and
where it comes from instead:

    bash ~/.local/share/icons/KDE-Modern-Classic/check-app-icons.sh

Paste the `breeze` and `hicolor` lines into a comment on the store page; the icon names are exactly what's
needed to add them.

## How it was made

Every icon in this theme was drawn as SVG by an AI assistant (Claude, by Anthropic), working from the
original KDE Classic icons under the author's direction. Each icon was reviewed by eye against the
originals and revised until it was right; many went through several rounds. The theme's design
decisions and final say on every icon are the author's.

## For distributions and theme bundles

KDE Modern Classic can be bundled with other projects, for example a Global Theme such as Klassik.

- **Licence:** LGPL-3.0-or-later (SPDX: `LGPL-3.0-or-later`), which can be shipped inside GPL-3.0 projects.
  Keep `COPYING`, `COPYING.LESSER`, `LICENSE-KDE-Classic.txt`, `AUTHORS` and this README with the icons.
- **Folder name:** `KDE-Modern-Classic`. That is the name a Global Theme refers to, for example in its
  `contents/defaults`:

      [kdeglobals][Icons]
      Theme=KDE-Modern-Classic

- **Inherits:** `kdeclassic,breeze,hicolor`. If the KDE Classic bitmap theme isn't installed, Plasma simply
  skips it and falls back to Breeze.
- **AI assistance:** please keep the "How it was made" section so users of the bundle see it too.
- **Updates:** take the icons from the latest release on the store page, so fixes and community redraws reach
  your users as well.

## Licence

KDE Modern Classic is released under the GNU Lesser General Public License, version 3 or (at your
option) any later version. See COPYING.LESSER, and COPYING for the GPL v3 text it builds on.

## Credits

- Author: Shaun Belcher (design direction, review, maintenance); see AUTHORS.
- KDE Classic artwork: the KDE artists of the 2.x/3.x era, maintained by James Richard Tyrer.
  Free for any use, including copying, modification and redistribution (LICENSE-KDE-Classic.txt).
  This theme reworks those designs.
- KDE logo (the K menu gear: start-here, kmenu and their aliases): from KDE_2_logo.svg, covered
  by the LGPL like KDE's own artwork.
- KDE, K Desktop Environment and the KDE Logo are trademarks of KDE e.V.
