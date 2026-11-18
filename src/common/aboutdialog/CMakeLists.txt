add_definitions(-DTRANSLATION_DOMAIN=\"com.github.neeeeow.klassik.aboutdialog\")

add_library(klassik_aboutdialog STATIC
    aboutdialog.cpp
)
qt_add_resources(klassik_aboutdialog "aboutdialog_resources"
    PREFIX "/klassik/aboutdialog"
    FILES about.png
    OUTPUT_TARGETS aboutdialog_resources_targets
)
set_target_properties(klassik_aboutdialog ${aboutdialog_resources_targets} PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(klassik_aboutdialog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(klassik_aboutdialog PRIVATE
    KLASSIK_VERSION="${PROJECT_VERSION}"
)
target_link_libraries(klassik_aboutdialog PUBLIC
    Qt::Widgets
    KF6::I18n
    KF6::KIOWidgets
)
