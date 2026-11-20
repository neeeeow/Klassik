# Service QMenu and Plasma::Applet
add_library(klassik_servicemenu STATIC
    servicemenu.cpp
    servicemenuapplet.cpp
    servicegroupmenu.cpp
    dirmenu.cpp
)
set_target_properties(klassik_servicemenu PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(klassik_servicemenu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(klassik_servicemenu PUBLIC
    klassik_panelmenu
    KF6::ConfigQml
)

# Button used to popup a servicemenu
add_library(com.github.neeeeow.klassik.servicemenu SHARED
    panelbuttonarrow.h
)
target_link_libraries(com.github.neeeeow.klassik.servicemenu PRIVATE
    klassik_painteditem
    Plasma::Plasma
)
ecm_add_qml_module(com.github.neeeeow.klassik.servicemenu
    URI "com.github.neeeeow.klassik.servicemenu"
    GENERATE_PLUGIN_SOURCE
)
ecm_target_qml_sources(com.github.neeeeow.klassik.servicemenu
    SOURCES
        qml/ServiceMenuButton.qml
)
ecm_finalize_qml_module(com.github.neeeeow.klassik.servicemenu DESTINATION ${KDE_INSTALL_QMLDIR})
install(TARGETS com.github.neeeeow.klassik.servicemenu ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
