add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_com.github.neeeeow.klassik.taskmanager\")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/metadata.json
    @ONLY
)

plasma_add_applet(com.github.neeeeow.klassik.taskmanager
    QML_SOURCES
        qml/ConfigAppearance.qml
        qml/ConfigBehavior.qml
        qml/config.qml
        qml/ContextMenu.qml
        qml/GroupMenu.qml
        qml/GroupContextMenu.qml
        qml/main.qml
        qml/Task.qml
        qml/code/LayoutMetrics.js
        qml/code/TaskTools.js
        ${KLASSIK_COMMON_DIR}/KlassikBusyIndicator.qml
    CPP_SOURCES
        taskbackground.h
        ${KLASSIK_COMMON_DIR}/sunkenappletframe.cpp
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
