add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_com.github.neeeeow.klassik.taskmanager\")

plasma_add_applet(com.github.neeeeow.klassik.taskmanager
    QML_SOURCES
        qml/ConfigAppearance.qml
        qml/ConfigBehavior.qml
        qml/config.qml
        qml/ContextMenu.qml
        qml/GroupMenu.qml
        qml/main.qml
        qml/MouseHandler.qml
        qml/PipeWireThumbnail.qml
        qml/PlayerController.qml
        qml/PulseAudio.qml
        qml/ScrollableTextWrapper.qml
        qml/TaskList.qml
        qml/Task.qml
        qml/ToolTipDelegate.qml
        qml/ToolTipInstance.qml
        qml/ToolTipWindowMouseArea.qml
        qml/code/LayoutMetrics.js
        qml/code/TaskTools.js
        ${KLASSIK_COMMON_DIR}/KlassikBusyIndicator.qml
    CPP_SOURCES
        backend.cpp
        taskbackground.h
        ${KLASSIK_COMMON_DIR}/sunkenappletframe.cpp
    RESOURCES
        main.xml
        ${KLASSIK_COMMON_DIR}/busyindicator.png
    GENERATE_APPLET_CLASS
)

kconfig_add_kcfg_files(com.github.neeeeow.klassik.taskmanager ${CMAKE_SOURCE_DIR}/applets/taskmanager/kactivitymanagerd_plugins_settings.kcfgc)

# FIXME Cleanup no longer used libs.
target_link_libraries(com.github.neeeeow.klassik.taskmanager PRIVATE
    klassik_painteditem
    Qt::Core
    Qt::Qml
    Qt::Quick
    Plasma::Activities
    Plasma::ActivitiesStats
    KF6::ConfigGui
    KF6::I18n
    KF6::KIOCore
    KF6::KIOGui
    KF6::KIOFileWidgets # KFilePlacesModel
    KF6::Notifications # KNotificationJobUiDelegate
    Plasma::Plasma
    KSysGuard::ProcessCore
    KF6::Service)
