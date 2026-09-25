cmake_minimum_required(VERSION 3.16)

project(klassik)

set(QT_MIN_VERSION "6.6.0")
set(KF6_MIN_VERSION "6.0.0")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)
include(FeatureSummary)
include(CheckIncludeFiles)
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
  ColorScheme
  JobWidgets
  Config
  CoreAddons
  Notifications
  IconThemes
)

find_package(Plasma REQUIRED)
find_package(PlasmaActivitiesStats REQUIRED)
find_package(PlasmaActivities REQUIRED)
find_package(PlasmaQuick REQUIRED)
find_package(LibKWorkspace REQUIRED)
find_package(KSysGuard REQUIRED)
find_package(LibNotificationManager REQUIRED)
find_package(KDecoration3 REQUIRED)

add_subdirectory(src)
