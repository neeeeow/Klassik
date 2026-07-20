plasma_add_applet(com.github.neeeeow.klassik.panel
    QML_SOURCES
        AppletContainer.qml
        ConfigOverlay.qml
        main.qml
        LayoutManager.js
    CPP_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/klassikpainteditem.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/klassikqstyleitem.h
        ${CMAKE_CURRENT_SOURCE_DIR}/../common/panelbackground.h
    RESOURCES
        main.xml
    GENERATE_CONTAINMENT_CLASS
)

target_include_directories(com.github.neeeeow.klassik.panel PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../common/
)

target_link_libraries(com.github.neeeeow.klassik.panel PRIVATE
  Qt::Core
  Qt::Widgets
  Qt::Qml
  Qt::Quick)

plasma_install_package(layout-template com.github.neeeeow.klassik.panel.default layout-templates layout-template)
