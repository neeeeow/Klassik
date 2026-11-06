#!/usr/bin/env bash
# check-app-icons.sh - find installed apps whose menu icon does NOT come from your current icon theme.
# Written for KDE Modern Classic, but works with any icon theme on KDE Plasma.
#
# It reads every app's .desktop file (system, your own, Flatpak and Snap), takes its Icon= name and asks KDE
# (kiconfinder6) which file it would actually use - the same lookup the menu, taskbar and title bars use.
#
# Usage:  bash check-app-icons.sh            # apps whose icon isn't from your theme
#         bash check-app-icons.sh --all      # every app, with where its icon comes from
#
# Columns: where the icon comes from | icon name | app name
#   breeze      falls back to Breeze                      <- worth reporting
#   hicolor     the app's own icon (often Breeze-styled)  <- worth reporting if it looks out of place
#   kdeclassic  the original KDE Classic bitmaps
#   file        the app points at a fixed image file - no icon theme can change these
#   other       another installed theme
#   missing     no icon found at all
#
# Reporting (KDE Modern Classic): paste the "breeze" and "hicolor" lines into a comment on the store page.
# The icon name column is exactly what's needed to add them. Other apps' brand logos (Firefox, GIMP, VLC ...)
# are left as they are on purpose.

ALL=0; [ "$1" = "--all" ] && ALL=1
command -v kiconfinder6 >/dev/null || { echo "kiconfinder6 not found (it comes with KDE Frameworks 6)."; exit 1; }
THEME=$(kreadconfig6 --file kdeglobals --group Icons --key Theme 2>/dev/null)
THEME=${THEME:-breeze}

dirs=(/usr/share/applications /usr/local/share/applications "$HOME/.local/share/applications"
      /var/lib/flatpak/exports/share/applications "$HOME/.local/share/flatpak/exports/share/applications"
      /var/lib/snapd/desktop/applications)

declare -A seen
out=$(
for d in "${dirs[@]}"; do
  [ -d "$d" ] || continue
  for f in "$d"/*.desktop; do
    [ -f "$f" ] || continue
    grep -qiE '^(NoDisplay|Hidden)=true' "$f" && continue          # not shown in the menu
    icon=$(grep -m1 '^Icon=' "$f" | cut -d= -f2-)
    name=$(grep -m1 '^Name=' "$f" | cut -d= -f2-)
    [ -z "$icon" ] && continue
    [ -n "${seen[$icon]}" ] && continue
    seen[$icon]=1
    if [[ "$icon" == /* ]]; then
      src="file"
    else
      path=$(kiconfinder6 "$icon" 2>/dev/null | head -1)
      case "$path" in
        "")                 src="missing" ;;
        *"/$THEME/"*)       src="theme" ;;
        *"/kdeclassic/"*)   src="kdeclassic" ;;
        *"/breeze"*)        src="breeze" ;;
        *"/hicolor/"*)      src="hicolor" ;;
        *)                  src="other" ;;
      esac
    fi
    if [ $ALL = 1 ] || [ "$src" != "theme" ]; then
      printf '%-10s  %-40s  %s\n' "$src" "$icon" "$name"
    fi
  done
done | sort
)

echo "Icon theme: $THEME"
echo
[ -n "$out" ] && echo "$out" || echo "Every app in your menu uses an icon from $THEME."
echo
echo "Summary: $(echo "$out" | grep -c '^breeze') from Breeze, $(echo "$out" | grep -c '^hicolor') app's own (hicolor), \
$(echo "$out" | grep -c '^file') fixed files."
