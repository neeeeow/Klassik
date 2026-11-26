add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_com.github.neeeeow.klassik.clock\")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/metadata.json
    @ONLY
)

plasma_add_applet(com.github.neeeeow.klassik.clock
    QML_SOURCES
        qml/main.qml
        qml/config.qml
        qml/ConfigGeneral.qml
        qml/ConfigTimeZones.qml
    CPP_SOURCES
        clock.cpp
        clipboardmenu.cpp
    RESOURCES
        main.xml
        lcd.png
    GENERATE_APPLET_CLASS
)  

target_link_libraries(com.github.neeeeow.klassik.clock PRIVATE
    klassik_painteditem
    Qt::Core
    Qt::Widgets
    Qt::Qml
    Qt::Quick
    KF6::I18n
)
