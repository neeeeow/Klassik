file(GLOB COLORS "${CMAKE_CURRENT_SOURCE_DIR}/*.colors")
if(COLORS)
    install(
        FILES ${COLORS}
        DESTINATION ${KDE_INSTALL_DATADIR}/color-schemes
    )
endif()
