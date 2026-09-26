set(KLASSIK_COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/common) # directory for common files

add_subdirectory(common)
add_subdirectory(applets)
add_subdirectory(panel)
add_subdirectory(kwin)
add_subdirectory(qstyle)
add_subdirectory(color-schemes)
add_subdirectory(wallpapers)

# Install plasma style and global theme directly here
plasma_install_package(plasmastyle klassik desktoptheme desktoptheme)
plasma_install_package(lookandfeel com.github.neeeeow.klassik.desktop look-and-feel lookandfeel)
