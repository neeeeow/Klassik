add_definitions(-DTRANSLATION_DOMAIN=\"com.github.neeeeow.klassik.aboutdialog\")

add_library(klassik_aboutdialog STATIC
    aboutdialog.cpp
)
set_target_properties(klassik_aboutdialog PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(klassik_aboutdialog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(klassik_aboutdialog PRIVATE
    KLASSIK_VERSION="${PROJECT_VERSION}"
)
target_link_libraries(klassik_aboutdialog PUBLIC
    Qt::Widgets
    KF6::I18n
)
