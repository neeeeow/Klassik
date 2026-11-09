add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_com.github.neeeeow.klassik.panel.icon\")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/metadata.json
    @ONLY
)

plasma_add_applet(com.github.neeeeow.klassik.panel.icon
    CPP_SOURCES
        iconapplet.cpp
        dirmenu.cpp
        ${KLASSIK_COMMON_DIR}/painteditems/styledframe.h
        ${KLASSIK_COMMON_DIR}/painteditems/panelbuttonarrow.h
    QML_SOURCES
        qml/main.qml
        ${KLASSIK_COMMON_DIR}/menu/servicemenu/qml/ServiceMenuButton.qml
    RESOURCES
        main.xml
)

target_link_libraries(com.github.neeeeow.klassik.panel.icon PRIVATE
    klassik_painteditem
    klassik_actions
    klassik_servicemenu
)
