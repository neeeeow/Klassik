# Klassik
A modern pixel-perfect recreation of the classic KDE 3 desktop experience for KDE Plasma 6, built entirely using Qt6 and Qt Quick with full support for Wayland and fractional scaling.

![Desktop screenshot](screenshots/desktop.png)

## Features
- **Classic panel applets:** Reimplementations of various KDE 3 panel applets, written in C++ and QML, all of which utilize `QStyle` and `QPainter` for drawing, ensuring they look as close to the original KDE 3 applets as possible. Currently included:
  - K Menu (application menu)
  - Quicklaunch
  - Task manager
  - Lock / log out buttons
  - Digital / analog clock
- A custom panel containment which uses `QStyle` and `QPainter` to draw the panel background, featuring support for custom backgrounds.
- Default KDE 2/3 `QStyle` theme, rewritten in Qt 6.
- Ports of old KDE 3 `KWin` themes to `KDecoration3`. Currently included:
  - KDE 2
- Color schemes included in KDE 3
- Wallpapers used in KDE 3
