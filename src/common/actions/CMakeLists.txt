# C++ files for the actions themselves
add_library(klassik_actions STATIC
    popuptitle.cpp
)
set_target_properties(klassik_actions PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(klassik_actions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(klassik_actions PUBLIC
    Qt::Core
    Qt::Widgets
)

# QML action factory
add_library(com.github.neeeeow.klassik.actionfactory SHARED
    actionfactory.cpp
)

target_link_libraries(com.github.neeeeow.klassik.actionfactory PRIVATE
    klassik_actions
)
ecm_add_qml_module(com.github.neeeeow.klassik.actionfactory
    URI "com.github.neeeeow.klassik.actionfactory"
    GENERATE_PLUGIN_SOURCE
)
ecm_finalize_qml_module(com.github.neeeeow.klassik.actionfactory DESTINATION ${KDE_INSTALL_QMLDIR})
install(TARGETS com.github.neeeeow.klassik.actionfactory ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
