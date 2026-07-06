cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(plasmoid-klassik-taskmanager)

set(KF6_MIN_VERSION "6.0.0")

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)
include(ECMQmlModule)

find_package(Qt6 REQUIRED COMPONENTS
  Core
  Widgets
  Qml
  Quick
)

find_package(KF6 REQUIRED COMPONENTS
  I18n
  Service
  KIO
  Config
  CoreAddons
  Notifications
)

find_package(Plasma REQUIRED)
find_package(PlasmaActivitiesStats REQUIRED)
find_package(PlasmaActivities REQUIRED)
find_package(PlasmaQuick REQUIRED)
find_package(KSysGuard REQUIRED)
find_package(LibNotificationManager REQUIRED)

add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_com.github.neeeeow.klassik.taskmanager\")

plasma_add_applet(com.github.neeeeow.klassik.taskmanager
    QML_SOURCES
        qml/AudioStream.qml
        qml/ConfigAppearance.qml
        qml/ConfigBehavior.qml
        qml/config.qml
        qml/ContextMenu.qml
        qml/GroupDialog.qml
        qml/GroupExpanderOverlay.qml
        qml/main.qml
        qml/MouseHandler.qml
        qml/PipeWireThumbnail.qml
        qml/PlayerController.qml
        qml/PulseAudio.qml
        qml/ScrollableTextWrapper.qml
        qml/TaskBadgeOverlay.qml
        qml/TaskList.qml
        qml/TaskProgressOverlay.qml
        qml/Task.qml
        qml/ToolTipDelegate.qml
        qml/ToolTipInstance.qml
        qml/ToolTipWindowMouseArea.qml
        qml/code/LayoutMetrics.js
        qml/code/TaskTools.js
    CPP_SOURCES
        backend.cpp
        smartlauncherbackend.cpp
        smartlauncheritem.cpp
    RESOURCES
        main.xml
    GENERATE_APPLET_CLASS
)

kconfig_add_kcfg_files(com.github.neeeeow.klassik.taskmanager ${CMAKE_SOURCE_DIR}/kactivitymanagerd_plugins_settings.kcfgc)

ecm_qt_declare_logging_category(com.github.neeeeow.klassik.taskmanager
    HEADER log_settings.h
    IDENTIFIER TASKMANAGER_DEBUG
    CATEGORY_NAME com.github.neeeeow.klassik.taskmanager)

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
