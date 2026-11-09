add_library(klassik_panelmenu STATIC
    panelmenu.cpp
)
set_target_properties(klassik_panelmenu PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(klassik_panelmenu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(klassik_panelmenu PUBLIC
    klassik_actions
    Qt::Core
    Qt::Widgets
    Qt::Quick
    KF6::I18n
    KF6::Service
    KF6::KIOWidgets
    KF6::ConfigQml
    Plasma::Activities
    Plasma::PlasmaQuick
)
