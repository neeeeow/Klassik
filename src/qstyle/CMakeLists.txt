add_library(klassik-style MODULE
  klassikstyle.cpp
  klassikstyleplugin.cpp
)

set_target_properties(klassik-style PROPERTIES
  OUTPUT_NAME com.github.neeeeow.klassik.qstyle
)

target_link_libraries(klassik-style PRIVATE
  Qt6::Widgets
)

install(TARGETS klassik-style DESTINATION ${KDE_INSTALL_QTPLUGINDIR}/styles/)
