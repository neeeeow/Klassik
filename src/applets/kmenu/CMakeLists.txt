add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_com.github.neeeeow.klassik.kmenu\")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/metadata.json
    @ONLY
)

plasma_add_applet(com.github.neeeeow.klassik.kmenu
    QML_SOURCES
        qml/ConfigGeneral.qml
        qml/config.qml
        qml/main.qml
        ${KLASSIK_COMMON_DIR}/qml/PanelButton.qml
    CPP_SOURCES
        kmenu.cpp
        kmenuapplet.cpp
        recentdocsmenu.cpp
        settingsmenu.cpp
        systemmenu.cpp
        kmenutitleaction.h
        kmenusearchaction.cpp
        ${KLASSIK_COMMON_DIR}/painteditems/styledframe.h
        ${KLASSIK_COMMON_DIR}/painteditems/panelbuttonarrow.h
    RESOURCES
        main.xml
        img/kside.png
        img/kside_tile.png
)  

target_link_libraries(com.github.neeeeow.klassik.kmenu PRIVATE
    klassik_servicemenu
    klassik_painteditem
    KF6::KIOFileWidgets
    Plasma::ActivitiesStats
    PW::KWorkspace
)

