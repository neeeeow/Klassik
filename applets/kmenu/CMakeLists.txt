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
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/klassikpainteditem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/klassikqstyleprimitiveitem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/panelbuttonbackground.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/panelbuttonarrow.h
  RESOURCES
    main.xml
    img/kside.png
    img/kside_tile.png
)  

target_include_directories(com.github.neeeeow.klassik.kmenu PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/
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

