configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/metadata.json
    @ONLY
)

plasma_add_applet(com.github.neeeeow.klassik.quicklaunch
    QML_SOURCES
        ConfigGeneral.qml
        config.qml
        IconItem.qml
        main.qml
        LauncherModel.qml
        layout.js
    CPP_SOURCES
        quicklaunch_p.cpp
        ${KLASSIK_COMMON_DIR}/styledframe.cpp
    RESOURCES
        main.xml
    GENERATE_APPLET_CLASS
)

target_link_libraries(com.github.neeeeow.klassik.quicklaunch PRIVATE
    klassik_painteditem
    Qt::Quick
    KF6::KIOWidgets
    KF6::JobWidgets)
