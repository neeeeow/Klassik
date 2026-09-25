var panel = new Panel("com.github.neeeeow.klassik.panel")
var panelScreen = panel.screen

panel.height = 48
panel.floating = false

panel.addWidget("com.github.neeeeow.klassik.kmenu")
panel.addWidget("com.github.neeeeow.klassik.quicklaunch")
panel.addWidget("org.kde.plasma.pager")
panel.addWidget("com.github.neeeeow.klassik.taskmanager")
panel.addWidget("com.github.neeeeow.klassik.lockout")
panel.addWidget("org.kde.plasma.systemtray")
panel.addWidget("com.github.neeeeow.klassik.clock")
