plasma_add_applet(com.github.neeeeow.klassik.panel
    QML_SOURCES
        AppletContainer.qml
        ConfigOverlay.qml
        main.qml
        LayoutManager.js
    RESOURCES
        main.xml
    GENERATE_CONTAINMENT_CLASS
)

plasma_install_package(layout-template com.github.neeeeow.klassik.panel.default layout-templates layout-template)  
