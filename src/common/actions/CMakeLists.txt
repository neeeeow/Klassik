add_library(klassik_actions STATIC
    popuptitle.cpp
)
set_target_properties(klassik_actions PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(klassik_actions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(klassik_actions PUBLIC
    Qt::Core
    Qt::Widgets
)
