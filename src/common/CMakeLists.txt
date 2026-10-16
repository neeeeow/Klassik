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

add_subdirectory(painteditems)
add_subdirectory(actions)
