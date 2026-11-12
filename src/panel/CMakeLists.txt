add_definitions(-DTRANSLATION_DOMAIN=\"plasma_containment_com.github.neeeeow.klassik.panel\")

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
        addappsmenu.cpp
        ${KLASSIK_COMMON_DIR}/painteditems/styledtoolbarhandle.h
        ${KLASSIK_COMMON_DIR}/painteditems/styledarrow.h
        ${KLASSIK_COMMON_DIR}/painteditems/styledframe.h
    RESOURCES
        main.xml
        defaultBackground.png
)

target_link_libraries(com.github.neeeeow.klassik.panel PRIVATE
    klassik_painteditem
    klassik_panelmenu
    Qt::Qml
    Plasma::Plasma
)

plasma_install_package(layout-template com.github.neeeeow.klassik.panel.default layout-templates layout-template)
