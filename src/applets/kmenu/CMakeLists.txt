add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_com.github.neeeeow.klassik.kmenu\")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/metadata.json
    @ONLY
)

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
        systemmenu.cpp
        popupmenutitle.h
        popupmenusearch.h
        ${KLASSIK_COMMON_DIR}/sunkenappletframe.cpp
        ${KLASSIK_COMMON_DIR}/panelbuttonarrow.cpp
    RESOURCES
        main.xml
        img/kside.png
        img/kside_tile.png
)  

target_link_libraries(com.github.neeeeow.klassik.kmenu PRIVATE
    klassik_painteditem
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
    KF6::KIOFileWidgets
    Plasma::Plasma
    Plasma::ActivitiesStats
    Plasma::Activities
    Plasma::PlasmaQuick
    PW::KWorkspace
)

