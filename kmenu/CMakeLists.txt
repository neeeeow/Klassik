cmake_minimum_required(VERSION 3.16)

project(plasmoid-klassik-kmenu)

find_package(ECM 1.4.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

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
)

find_package(Plasma REQUIRED COMPONENTS) # Required for plasma_install_package
find_package(PlasmaActivitiesStats REQUIRED)

plasma_install_package(package com.github.neeeeow.klassik.kmenu)

add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_com.github.neeeeow.klassik.kmenu\")

set(kmenuplugin_SRCS
  plugin/kmenu.cpp
  plugin/kmenumanager.cpp
  plugin/kmenuplugin.cpp
)

add_library(kmenuplugin SHARED ${kmenuplugin_SRCS})

target_link_libraries(kmenuplugin
  Qt::Core
  Qt::Widgets
  Qt::Qml
  Qt::Quick
  KF6::I18n
  KF6::Service
  KF6::KIOGui
  Plasma::ActivitiesStats
)

install(TARGETS kmenuplugin DESTINATION ${QT6_INSTALL_QML}/com/github/neeeeow/klassik/kmenu)
install(FILES plugin/qmldir DESTINATION ${QT6_INSTALL_QML}/com/github/neeeeow/klassik/kmenu)
