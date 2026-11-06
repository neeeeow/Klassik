# Changelog

## KDE Modern Classic – 2026-11-06

### New icons

- **TeX font files** (packed `.pk` and generic `.gf` fonts, TeX fonts and font metrics): KDE 3's grey cat's face on
  the page. Packed fonts carry the same magenta corner marks as compressed files. Before, they used the plain font
  icon.

### Improvements

- **Unknown files**: the hieroglyphs from the KDE 3 icon are back, carved into the page behind the question mark
  (two reed leaves, a basket and a standing falcon). The question mark is a little wider with its end curling in,
  and its dot is now a round green ball standing clear of the stem.
- **Perforations on text files**, as in KDE Classic: a column of punched slots down the right edge of the page,
  fading lighter towards the top, on plain text, source code, scripts, HTML, CSS, JSON, XML, YAML, Markdown,
  TeX, Makefile, README, subtitles, calendars, mail messages and other text-based files (55 types). Text lines and
  source-code dots on these pages stop short of the slots.
- **Documents, Music, Pictures and Videos folders** at 16 and 22 px (Dolphin's Places panel, Details view and
  file dialogs) now show just their emblem, filling the icon, like Home and Downloads. Before, the folder was
  still drawn behind them. Videos uses a single big film frame at these sizes, since the long film strip
  was too thin to read. The Cloud folder (also Google Drive, Dropbox, OneDrive, Nextcloud and
  ownCloud) shows just its cloud at these sizes, since on the folder it was too small to see.

## KDE Modern Classic – 2026-10-27

### New icons

- **Emoji Selector**: a winking smiley in front of a small card holding a heart, a star and a water drop. Before, it
  used the Icons settings icon.
- **qBittorrent** (with "Use icons from system theme" turned on): all of its toolbar, status bar, filter and list
  icons now match the theme instead of falling back to its own flat icons. New drawings for downloading, seeding,
  stalled, the status filters, peers (add and remove), connected and disconnected, alternative speed limits, queued
  and set location; the rest reuse the theme's existing icons. Start and Stop on its toolbar use the round green
  play and amber pause buttons. qBittorrent's own logo is left as it is.
- **Gwenview**: a new app icon, the pink eye with a camera lens, replacing the blue eye from 2026-10-26.
- **USB thumb drives** (`drive-removable-media-usb-pendrive`): a full-size blue USB stick, also used on the USB
  Devices settings page, with the plug's two holes side by side. Other USB drives keep the grey drive box.
- **Share** (the Share menu and button in Gwenview, Dolphin, Spectacle, Okular and others): a document page with
  teal linked nodes, and just the linked nodes at menu and toolbar sizes (16 and 22 px) so it stays clear. It used KDE Classic's plug, which means "shared on the network"; the plug stays as the badge on
  shared folders.
- **Magnet links** (`kt-magnet`): a red horseshoe magnet, used by qBittorrent and KTorrent.
- **Statistics** (`view-statistics`) and **Verify** (`document-edit-verify`): these KDE names were coming from Breeze
  and from an old 16 px KDE Classic image. Any app that uses them now gets the theme's pie chart and a page with a
  green tick.

### Improvements

- **Icons settings page** redrawn after KDE Classic's original: two overlapping postage stamps, a plum one behind and
  a green one in front, each with a queen's head in profile. The queen outline was drawn by Shaun Belcher.
- **Notification bell**: the wooden handle is now a slim turned oval on a small bead, closer to the KDE 3 bell,
  instead of a straight cylinder. This shows on the Notifications settings page, every tray bell (active, quiet,
  unread, off) and the appointment, task and note reminder icons.
- **Drawing files** (SVG, EPS, Illustrator, WMF/EMF, OpenDocument Drawing and LibreOffice Draw): the teal set
  square is smaller and sits over the page, closer to the KDE Classic original, with a slimmer angled edge.
- **Speaker** redrawn after the KDE Classic original: a wider speaker with a long teal cone, a pointed silver dome
  at the centre and a finer outline. This shows on audio files, the volume control in the panel, Speakers, the
  Sound settings page, the Music folder and Kid3.
- **Sound waves** on the volume control, Speakers and the Sound settings page are now gentle arcs that stand clear
  of the speaker, as in most icon themes, instead of half circles. On the panel's volume icon (and Muted and
  Speakers) the speaker is drawn a quarter larger so it reads clearly at panel size.
- **"New" actions** (New Window, New Tab, New Bookmark, New Message, New from Template, New Appointment, Task,
  Journal and Contact, New Tag, New Layer): the yellow sparkle is now a green plus badge, which reads much better
  at small sizes and matches New Group. The sparkle stays on the Wizard and code class icons, where it isn't "new".
- **Add and Remove** (the + and − on buttons, menus and toolbars across KDE: System Settings lists, Dolphin's
  Create New, Kate, KMail, Elisa playlists and more): now a green plus and a red minus badge. The old black symbols
  almost disappeared on dark colour schemes. Feedback welcome: the old design is kept and can be brought back.

### Fixes

- **Create New > Empty File** in Dolphin and file dialogs now shows the theme's blank page instead of Breeze's
  icon (KIO asks for an icon called `none` there).
- **Firefox keeps its own logo again.** The generic browser icon added on 2026-10-26 was hiding Firefox's logo, so
  it has been taken out of the theme. It is now a separate optional add-on, "KDE Modern Classic (Generic Browser)",
  for anyone who prefers it. If you installed 2026-10-26, remove it before installing this version, then clear
  `~/.cache/icon-cache.kcache`.

## KDE Modern Classic – 2026-10-26

### New icons

- **LibreOffice**: Writer, Calc, Impress, Draw, Base and Math now have their own app icons. Before, they used the
  document-type icons. The Start Center icon has been refreshed to match. Older versioned names (6.4, 7.0, 24.2,
  25.2) are covered too.
- **App icons requested by the community:**
  - Gwenview: an eye.
  - KTorrent: a blue arrow with a splash.
  - Kid3: the classic speaker with a music tag.
  - QDBusViewer: "DV" in a green box.
  - Root Terminal: Konsole's monitor with a padlock.
  - mpv: a purple play button.
  - Firefox: a generic browser window. When Firefox is installed, its own icon is still used.
- **Network folders (SMB / Windows shares)**: a new icon showing a globe on a network junction. The plain globe is
  kept for web and internet icons.

### Improvements

- **Printer redrawn**: a new 3D printer with a rounded lid, paper feeding through the top slot and a paper tray.
  Print, Print Preview, the printer status icons and the printer folder all use it.
- **Dolphin view modes** (Icons, Compact, Details, Tree): new versions drawn for 16 and 22 px, so they're easy to
  tell apart at small sizes and on dark themes.

### Licence

- Added an AUTHORS file with the licence notice (LGPL-3.0-or-later). The README now has a section for distributions
  and theme bundles.
