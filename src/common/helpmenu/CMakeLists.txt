add_definitions(-DTRANSLATION_DOMAIN=\"com.github.neeeeow.klassik.helpmenu\")

add_library(klassik_helpmenu STATIC
    helpmenu.cpp
    aboutdialog.cpp
)
qt_add_resources(klassik_helpmenu "helpmenu_resources"
    PREFIX "/klassik/helpmenu"
    FILES about.png
    OUTPUT_TARGETS helpmenu_resources_targets
)
set_target_properties(klassik_helpmenu ${helpmenu_resources_targets} PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
target_include_directories(klassik_helpmenu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(klassik_helpmenu PRIVATE
    KLASSIK_VERSION="${PROJECT_VERSION}"
)
target_link_libraries(klassik_helpmenu PUBLIC
    Qt::Widgets
    KF6::I18n
    KF6::KIOWidgets
    KF6::XmlGui
)
