cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(plasmoid-klassik-kmenu)

set(KF6_MIN_VERSION "6.0.0")
find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

# Find required packages

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
)

find_package(Plasma REQUIRED) # Required for plasma_install_package
find_package(PlasmaActivitiesStats REQUIRED)
find_package(PlasmaActivities REQUIRED)
find_package(PlasmaQuick REQUIRED)
find_package(LibKWorkspace REQUIRED)

add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_com.github.neeeeow.klassik.kmenu\")

plasma_add_applet(com.github.neeeeow.klassik.kmenu
  QML_SOURCES
    qml/ConfigGeneral.qml
    qml/config.qml
    qml/main.qml
  CPP_SOURCES
    containmentinterface.cpp
    servicemenu.cpp
    kmenu.cpp
    kmenuapplet.cpp
    recentdocsmenu.cpp
    settingsmenu.cpp
    popupmenutitle.h
    popupmenusearch.h
  RESOURCES
    main.xml
    img/kside.png
    img/kside_tile.png
)  

target_link_libraries(com.github.neeeeow.klassik.kmenu PRIVATE
  Qt::Core
  Qt::Widgets
  Qt::Qml
  Qt::Quick
  KF6::I18n
  KF6::Service
  KF6::KIOGui
  KF6::ColorScheme
  KF6::JobWidgets
  KF6::KIOWidgets
  KF6::ConfigGui
  KF6::ConfigQml
  Plasma::Plasma
  Plasma::ActivitiesStats
  Plasma::Activities
  Plasma::PlasmaQuick
  PW::KWorkspace
)

