configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/metadata.json
    @ONLY
)

plasma_add_applet(com.github.neeeeow.klassik.panel
    QML_SOURCES
        qml/ConfigDialog.qml
        qml/main.qml
        qml/LayoutManager.js
    CPP_SOURCES
        panelcontainment.cpp
        panelbackgroundimage.cpp
        grabhandle.h
        handlearrow.h
        ${KLASSIK_COMMON_DIR}/styledframe.cpp
    RESOURCES
        main.xml
        defaultBackground.png
)

target_link_libraries(com.github.neeeeow.klassik.panel PRIVATE
    klassik_painteditem
    Qt::Core
    Qt::Widgets
    Qt::Qml
    Qt::Quick
    Plasma::Plasma
)

plasma_install_package(layout-template com.github.neeeeow.klassik.panel.default layout-templates layout-template)
