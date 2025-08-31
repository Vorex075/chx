cmake_minimum_required(VERSION 3.28)
project(chx VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(chx INTERFACE)
add_library(chx::chx ALIAS chx)

target_include_directories(chx INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_compile_features(chx INTERFACE cxx_std_23)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS chx
  EXPORT chxTargets
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT chxTargets
  FILE chxTargets.cmake
  NAMESPACE chx::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/chx
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/chxConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/chxConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/chxConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/chx
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/chxConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/chxConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/chx
)


# ---- CPack: single archive, unzip-and-use ----
# Default to a relocatable prefix so the archive unpacks to: chx-<ver>/{include,lib/cmake/chx}
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "chx-${PROJECT_VERSION}" CACHE PATH "" FORCE)
endif()

set(CPACK_PACKAGE_NAME "chx")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_GENERATOR "TGZ;ZIP")

set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY ON)

# File naming
set(CPACK_PACKAGE_FILE_NAME
  "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}"
)

include(CPack)
