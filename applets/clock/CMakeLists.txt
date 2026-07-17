plasma_add_applet(com.github.neeeeow.klassik.clock
  QML_SOURCES
    qml/main.qml
    qml/config.qml
    qml/ConfigGeneral.qml
  CPP_SOURCES
    clock.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/klassikpainteditem.cpp
  RESOURCES
    main.xml
    lcd.png
  GENERATE_APPLET_CLASS
)  

target_link_libraries(com.github.neeeeow.klassik.clock PRIVATE
  Qt::Core
  Qt::Widgets
  Qt::Qml
  Qt::Quick
  KF6::ConfigQml
)
