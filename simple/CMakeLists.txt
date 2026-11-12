# set gettext domain, required by the M_() translation-marking macro
add_definitions(-DGETTEXT_DOMAIN=\"simple-libdnf5-plugin\")

# add the plugin source file
add_library(simple MODULE simple.cpp)

# disable the 'lib' prefix in order to create simple.so
set_target_properties(simple PROPERTIES PREFIX "")

# link the libdnf5 library
target_link_libraries(simple PUBLIC dnf5)

# install the plugin into the common libdnf5-plugins location
install(TARGETS simple LIBRARY DESTINATION "${CMAKE_INSTALL_FULL_LIBDIR}/libdnf5/plugins/")
install(FILES "simple.conf" DESTINATION "${CMAKE_INSTALL_FULL_SYSCONFDIR}/dnf/libdnf5-plugins")
