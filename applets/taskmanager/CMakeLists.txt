add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_com.github.neeeeow.klassik.taskmanager\")

plasma_add_applet(com.github.neeeeow.klassik.taskmanager
    QML_SOURCES
        qml/ConfigAppearance.qml
        qml/ConfigBehavior.qml
        qml/config.qml
        qml/ContextMenu.qml
        qml/GroupDialog.qml
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
    CPP_SOURCES
        backend.cpp
        taskbackground.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../../common/klassikpainteditem.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../../common/klassikqstyleitem.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../../common/sunkenappletframe.h
    RESOURCES
        main.xml
    GENERATE_APPLET_CLASS
)

target_include_directories(com.github.neeeeow.klassik.taskmanager PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../../common/
) # ensure that CMake sees our frame

kconfig_add_kcfg_files(com.github.neeeeow.klassik.taskmanager ${CMAKE_SOURCE_DIR}/applets/taskmanager/kactivitymanagerd_plugins_settings.kcfgc)

ecm_qt_declare_logging_category(com.github.neeeeow.klassik.taskmanager
    HEADER log_settings.h
    IDENTIFIER TASKMANAGER_DEBUG
    CATEGORY_NAME com.github.neeeeow.klassik.taskmanager
)

# FIXME Cleanup no longer used libs.
target_link_libraries(com.github.neeeeow.klassik.taskmanager PRIVATE
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
  KF6::Service
  KF6::WindowSystem
  PW::LibNotificationManager)
