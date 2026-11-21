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
    Qt::QuickPrivate
)

# The other generic painted items which get exposed to QML
add_library(com.github.neeeeow.klassik.painteditems SHARED
    styledarrow.h
    styledframe.h
    styledtoolbarhandle.h
    styledtoolbutton.h
    styledfocusrect.h
)
target_link_libraries(com.github.neeeeow.klassik.painteditems PRIVATE
    klassik_painteditem
)
ecm_add_qml_module(com.github.neeeeow.klassik.painteditems
    URI "com.github.neeeeow.klassik.painteditems"
    GENERATE_PLUGIN_SOURCE
)
ecm_target_qml_sources(com.github.neeeeow.klassik.painteditems
    SOURCES
        StyledToolButton.qml
        StyledToolBarHandle.qml
)

ecm_finalize_qml_module(com.github.neeeeow.klassik.painteditems DESTINATION ${KDE_INSTALL_QMLDIR})
install(TARGETS com.github.neeeeow.klassik.painteditems ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

