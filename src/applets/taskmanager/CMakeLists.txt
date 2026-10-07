add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_com.github.neeeeow.klassik.taskmanager\")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/metadata.json
    @ONLY
)

plasma_add_applet(com.github.neeeeow.klassik.taskmanager
    QML_SOURCES
        ConfigAppearance.qml
        ConfigBehavior.qml
        config.qml
        ContextMenu.qml
        GroupMenu.qml
        GroupContextMenu.qml
        main.qml
        Task.qml
        code/LayoutMetrics.js
        code/TaskTools.js
        ${KLASSIK_COMMON_DIR}/KlassikBusyIndicator.qml
    CPP_SOURCES
        taskbackground.h
        ${KLASSIK_COMMON_DIR}/styledframe.cpp
    RESOURCES
        main.xml
        ${KLASSIK_COMMON_DIR}/busyindicator.png
    GENERATE_APPLET_CLASS
)


target_link_libraries(com.github.neeeeow.klassik.taskmanager PRIVATE
    klassik_painteditem
    Qt::Core
    Qt::Qml
    Qt::Quick)
