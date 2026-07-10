plasma_add_applet(com.github.neeeeow.klassik.clock
  QML_SOURCES
    qml/main.qml
    qml/config.qml
    qml/ConfigGeneral.qml
  CPP_SOURCES
    digitalclock.cpp
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
