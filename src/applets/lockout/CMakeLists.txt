configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/metadata.json
    @ONLY
)

plasma_add_applet(com.github.neeeeow.klassik.lockout
    QML_SOURCES
        main.qml
    CPP_SOURCES
        ${KLASSIK_COMMON_DIR}/painteditems/styledtoolbutton.h
    GENERATE_APPLET_CLASS
)

target_link_libraries(com.github.neeeeow.klassik.lockout PRIVATE
    klassik_painteditem
)
