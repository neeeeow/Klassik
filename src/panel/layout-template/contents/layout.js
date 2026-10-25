const panel = new Panel("com.github.neeeeow.klassik.panel")

panel.height = 48
panel.floating = false

panel.addWidget("com.github.neeeeow.klassik.kmenu")

// Add icons
let icon = panel.addWidget("com.github.neeeeow.klassik.panel.icon")
icon.currentConfigGroup = ["General"]
icon.writeConfig("url", "file:///usr/share/applications/org.kde.dolphin.desktop")

icon = panel.addWidget("com.github.neeeeow.klassik.panel.icon")
icon.currentConfigGroup = ["General"]
icon.writeConfig("url", "file:///usr/share/applications/org.kde.konsole.desktop")

panel.addWidget("org.kde.plasma.pager")
panel.addWidget("com.github.neeeeow.klassik.taskmanager")
panel.addWidget("com.github.neeeeow.klassik.lockout")
panel.addWidget("org.kde.plasma.systemtray")
panel.addWidget("com.github.neeeeow.klassik.clock")
