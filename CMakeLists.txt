cmake_minimum_required(VERSION 3.16)

project(klassik VERSION 1.0.1)

set(QT_MIN_VERSION "6.6.0")
set(KF6_MIN_VERSION "6.9.0")
set(PLASMA_MIN_VERSION "6.4.0")

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

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS
  Core
  Widgets
  Qml
  Quick
  QuickPrivate
)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
  I18n
  Service
  KIO
  Config
  CoreAddons
  IconThemes
  Notifications
  XmlGui
  WidgetsAddons
)

find_package(Plasma ${PLASMA_MIN_VERSION} REQUIRED)
find_package(PlasmaActivitiesStats ${PLASMA_MIN_VERSION} REQUIRED)
find_package(PlasmaActivities ${PLASMA_MIN_VERSION} REQUIRED)
find_package(PlasmaQuick ${PLASMA_MIN_VERSION} REQUIRED)
find_package(LibKWorkspace ${PLASMA_MIN_VERSION} REQUIRED)
find_package(KDecoration3 ${PLASMA_MIN_VERSION} REQUIRED)

add_subdirectory(src)
