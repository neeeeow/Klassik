plasma_add_applet(com.github.neeeeow.klassik.quicklaunch
    QML_SOURCES
        ConfigGeneral.qml
        config.qml
        IconItem.qml
        main.qml
        UrlModel.qml
        layout.js
    CPP_SOURCES
        quicklaunch_p.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/klassikpainteditem.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/klassikqstyleprimitiveitem.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/panelbuttonbackground.h
    RESOURCES
        main.xml
    GENERATE_APPLET_CLASS
)

target_include_directories(com.github.neeeeow.klassik.quicklaunch PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/../common/
)

target_link_libraries(com.github.neeeeow.klassik.quicklaunch PRIVATE
  Qt::Quick
  KF6::KIOWidgets
  KF6::JobWidgets)
