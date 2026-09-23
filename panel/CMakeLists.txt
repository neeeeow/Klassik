plasma_add_applet(com.github.neeeeow.klassik.panel
    QML_SOURCES
        AppletContainer.qml
        ConfigDialog.qml
        main.qml
        LayoutManager.js
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/KlassikBusyIndicator.qml
    CPP_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/painteditem.cpp
        panelbackground.cpp
        grabhandle.h
        handlearrow.h
    RESOURCES
        main.xml
        defaultBackground.png
    GENERATE_CONTAINMENT_CLASS
)

target_include_directories(com.github.neeeeow.klassik.panel PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../common/
)

target_link_libraries(com.github.neeeeow.klassik.panel PRIVATE
  Qt::Core
  Qt::Widgets
  Qt::Qml
  Qt::Quick
  KF6::ColorScheme
  Plasma::Plasma
)

plasma_install_package(layout-template com.github.neeeeow.klassik.panel.default layout-templates layout-template)
