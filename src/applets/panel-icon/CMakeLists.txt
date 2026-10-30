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
        ${KLASSIK_COMMON_DIR}/qml/PanelButton.qml
    RESOURCES
        main.xml
)

target_link_libraries(com.github.neeeeow.klassik.panel.icon PRIVATE
    klassik_painteditem
    klassik_actions
    klassik_servicemenu
    KF6::I18n
    KF6::KIOCore # for OpenFileManagerWindowJob
    KF6::KIOGui # for FavIconRequestJob
    KF6::KIOWidgets # for KRun
    KF6::Notifications
    Plasma::Plasma
)
