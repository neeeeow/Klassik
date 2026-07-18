var panel = new Panel("com.github.neeeeow.klassik.panel")
var panelScreen = panel.screen

panel.height = 2 * Math.ceil(gridUnit * 2.5 / 2)

panel.addWidget("com.github.neeeeow.klassik.kmenu")
panel.addWidget("com.github.neeeeow.klassik.quicklaunch")
panel.addWidget("org.kde.plasma.pager")
panel.addWidget("com.github.neeeeow.klassik.taskmanager")

var langIds = ["as",    // Assamese
"bn",    // Bengali
"bo",    // Tibetan
"brx",   // Bodo
"doi",   // Dogri
"gu",    // Gujarati
"hi",    // Hindi
"ja",    // Japanese
"kn",    // Kannada
"ko",    // Korean
"kok",   // Konkani
"ks",    // Kashmiri
"lep",   // Lepcha
"mai",   // Maithili
"ml",    // Malayalam
"mni",   // Manipuri
"mr",    // Marathi
"ne",    // Nepali
"or",    // Odia
"pa",    // Punjabi
"sa",    // Sanskrit
"sat",   // Santali
"sd",    // Sindhi
"si",    // Sinhala
"ta",    // Tamil
"te",    // Telugu
"th",    // Thai
"ur",    // Urdu
"vi",    // Vietnamese
"zh_CN", // Simplified Chinese
"zh_TW"] // Traditional Chinese

if (langIds.indexOf(languageId) != -1) {
    panel.addWidget("org.kde.plasma.kimpanel");
}

panel.addWidget("org.kde.plasma.systemtray")
panel.addWidget("com.github.neeeeow.klassik.clock")
