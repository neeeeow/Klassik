add_library(kde2-decoration MODULE
  kde2.cpp
)

set_target_properties(kde2-decoration PROPERTIES
  OUTPUT_NAME com.github.neeeeow.klassik.kwin.kde2
)

target_link_libraries(kde2-decoration
  PRIVATE
  Qt6::DBus
  Qt6::Widgets
  KF6::CoreAddons
  KF6::IconThemes
  KDecoration3::KDecoration
)

install(TARGETS kde2-decoration DESTINATION ${KDE_INSTALL_PLUGINDIR}/${KDECORATION_PLUGIN_DIR})
