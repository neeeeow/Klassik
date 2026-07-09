plasma_add_applet(com.github.neeeeow.klassik.clock
  QML_SOURCES
    qml/main.qml
  CPP_SOURCES
    digitalclock.cpp
  RESOURCES
    lcd.png
  GENERATE_APPLET_CLASS
)  

target_link_libraries(com.github.neeeeow.klassik.clock PRIVATE
  Qt::Core
  Qt::Widgets
  Qt::Qml
  Qt::Quick
)
