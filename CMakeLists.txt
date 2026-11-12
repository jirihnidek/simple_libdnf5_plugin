message("Running CMake on simple libdnf5 plugin...")
cmake_minimum_required(VERSION 3.21)

project(simple-libdnf5-plugin LANGUAGES CXX VERSION 0.1.0)

message("Building ${PROJECT_NAME} version ${PROJECT_VERSION}")

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# warnings
add_compile_options(-Wall -Wextra -Werror)

# linker options
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # clang doesn't support this option
    add_link_options(-Wl,--as-needed)
endif()

# Use standard GNU install directories (CMAKE_INSTALL_FULL_LIBDIR, etc.)
include(GNUInstallDirs)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBDNF5 REQUIRED IMPORTED_TARGET libdnf5)

# libdnf5 plugin
add_subdirectory("simple")
