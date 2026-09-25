configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata.json.in
    ${CMAKE_CURRENT_BINARY_DIR}/metadata.json
    @ONLY
)

plasma_add_applet(com.github.neeeeow.klassik.lockout
    QML_SOURCES
        main.qml
    GENERATE_APPLET_CLASS
)
