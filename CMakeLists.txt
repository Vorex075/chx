cmake_minimum_required(VERSION 3.28)
project(chx VERSION 0.1 LANGUAGES CXX)

add_library(chx
  INTERFACE
)

target_compile_features(chx INTERFACE cxx_std_23)

include(GNUInstallDirs)


target_include_directories(chx
  INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_library(chx::chx ALIAS chx)

install(
  TARGETS chx
  EXPORT chxTargets
)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  EXPORT chxTargets
  NAMESPACE chx::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/chx
)

include(CMakePackageConfigHelpers)

# Genera el fichero de versionado para find_package
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/chxConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

# Configura el fichero chxConfig.cmake a partir de la plantilla
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/chxConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/chxConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/chx
)

# Instalar los ficheros de configuración
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/chxConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/chxConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/chx
)


option(CHX_ENABLE_TESTS "Build and run tests" ON)

if (CHX_ENABLE_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
