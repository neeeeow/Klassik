add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_com.github.neeeeow.klassik.servicegroupmenu\")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/metadata.json
    @ONLY
)

plasma_add_applet(com.github.neeeeow.klassik.servicegroupmenu
    CPP_SOURCES
        servicegroupmenuapplet.cpp
        ${KLASSIK_COMMON_DIR}/painteditems/styledframe.h
        ${KLASSIK_COMMON_DIR}/painteditems/panelbuttonarrow.h
    QML_SOURCES
        qml/main.qml
        ${KLASSIK_COMMON_DIR}/qml/PanelButton.qml
    RESOURCES
        main.xml
)

target_link_libraries(com.github.neeeeow.klassik.servicegroupmenu PRIVATE
    klassik_painteditem
    klassik_actions
    klassik_servicemenu
)
