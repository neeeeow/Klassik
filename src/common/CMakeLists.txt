# Precompile painteditem.cpp once to save time
add_library(klassik_painteditem STATIC
    painteditem.cpp
)
set_target_properties(klassik_painteditem PROPERTIES POSITION_INDEPENDENT_CODE ON)
qt6_extract_metatypes(klassik_painteditem)
target_include_directories(klassik_painteditem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(klassik_painteditem PUBLIC
    Qt::Core
    Qt::Widgets
    Qt::Quick
)

# Precompile panelpopupitem.cpp
add_library(klassik_panelpopupitem STATIC
    panelpopupitem.cpp
)
set_target_properties(klassik_panelpopupitem PROPERTIES POSITION_INDEPENDENT_CODE ON)
qt6_extract_metatypes(klassik_panelpopupitem)
target_include_directories(klassik_panelpopupitem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(klassik_panelpopupitem PUBLIC
    Qt::Core
    Qt::Widgets
    Qt::Quick
    Plasma::Plasma
)
