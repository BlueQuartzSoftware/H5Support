cmake_policy(SET CMP0002 NEW)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0079 NEW)
cmake_policy(SET CMP0077 NEW)

cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(H5Support LANGUAGES C CXX VERSION 1.0.4)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(H5Support_USE_MUTEX "Use mutex in functions" ON)
option(H5Support_INCLUDE_QT_API "Include support for using Qt classes with H5Lite" ON)

#------------------------------------------------------------------------------
# Add the H5Support Library Target and an Alias for the target
#------------------------------------------------------------------------------
add_library(H5Support INTERFACE)
add_library(H5Support::H5Support ALIAS H5Support)

target_compile_features(H5Support
  INTERFACE
    cxx_std_17
)

# Force HDF5 1.10 API
target_compile_definitions(H5Support INTERFACE "H5_USE_110_API")

if(H5Support_INCLUDE_QT_API)
  target_compile_definitions(H5Support INTERFACE H5Support_USE_QT)
endif()

if(H5Support_USE_MUTEX)
  target_compile_definitions(H5Support INTERFACE H5Support_USE_MUTEX)
endif()

set(H5Support_HDRS
  ${H5Support_SOURCE_DIR}/Source/H5Support/H5Lite.h
  ${H5Support_SOURCE_DIR}/Source/H5Support/H5Utilities.h
  ${H5Support_SOURCE_DIR}/Source/H5Support/H5ScopedSentinel.h
  ${H5Support_SOURCE_DIR}/Source/H5Support/H5ScopedErrorHandler.h
  ${H5Support_SOURCE_DIR}/Source/H5Support/H5Macros.h
  ${H5Support_SOURCE_DIR}/Source/H5Support/H5SupportTypeDefs.h
  ${H5Support_SOURCE_DIR}/Source/H5Support/H5Support.h
)

if(H5Support_INCLUDE_QT_API)
  list(APPEND H5Support_HDRS
    ${H5Support_SOURCE_DIR}/Source/H5Support/QH5Lite.h
    ${H5Support_SOURCE_DIR}/Source/H5Support/QH5Utilities.h
    ${H5Support_SOURCE_DIR}/Source/H5Support/QtBackwardsCompatibilityMacro.h
  )
endif()

# target_sources(H5Support
#   INTERFACE
#     ${H5Support_HDRS}
# )

source_group("H5Support" FILES ${H5Support_HDRS})

target_include_directories(H5Support
  INTERFACE
    $<BUILD_INTERFACE:${H5Support_SOURCE_DIR}/Source>
)

include(${H5Support_SOURCE_DIR}/Resources/CMake/TargetCopyInstall.cmake)

option(H5Support_INSTALL_HDF5 "Enable install of HDF5 dlls" OFF)
option(H5Support_INSTALL_QT5 "Enable install of Qt5 dlls" OFF)

set(HDF5_RULES COPY)
if(H5Support_INSTALL_HDF5)
  list(APPEND HDF5_RULES " " INSTALL)
endif()


#------------------------------------------------------------------------------
# Find the HDF5 Library
#------------------------------------------------------------------------------
if(SIMPL_EMBED_PYTHON) # -- HDF5 is located in a python virtual environment
  set(HDF5_USE_STATIC_LIBRARIES OFF)
  set(HDF5_FIND_COMPONENTS OFF)
  find_package(HDF5 1.10.6 EXACT MODULE REQUIRED)
  set(HDF5_C_TARGET_NAME hdf5::hdf5)
  target_compile_definitions(H5Support INTERFACE H5_BUILT_AS_DYNAMIC_LIB)
else() # -- Normal build with HDF5 found using HDF5Config.cmake
  #find_package(HDF5 NAMES hdf5 REQUIRED CONFIG)

  #------------------------------------------------------------------------------
  # If the developer has set another CMP directory then use that, otherwise look
  # for the CMP directory at the same level as the SIMPL directory
  if("${CMP_SOURCE_DIR}" STREQUAL "")
    set(CMP_SOURCE_DIR ${H5Support_SOURCE_DIR}/cmake)
    message(STATUS "CMP_SOURCE_DIR: ${CMP_SOURCE_DIR}")
  endif()

  if(NOT EXISTS "${CMP_SOURCE_DIR}")
    message(FATAL_ERROR "CMP_SOURCE_DIR does not exist at '${CMP_SOURCE_DIR}'")
  endif()

  # --------------------------------------------------------------------
  # Find HDF5 Headers/Libraries
  # HDF5 now comes with everything that is needed for CMake to load
  # up the targets (Exported) that it needs. We just need to find where
  # HDF5 is installed.
  include(${CMP_SOURCE_DIR}/ExtLib/HDF5Support.cmake)
  set(HDF5_C_TARGET_NAME hdf5::hdf5-shared)
endif()
target_include_directories(H5Support INTERFACE ${HDF5_INCLUDE_DIRS})
target_link_libraries(H5Support INTERFACE ${HDF5_C_TARGET_NAME})


#------------------------------------------------------------------------------
# Find the Qt5 Library if needed
#------------------------------------------------------------------------------
if(H5Support_USE_QT)
  set(QT5_RULES COPY)
  if(H5Support_INSTALL_QT5)
    list(APPEND QT5_RULES " " INSTALL)
  endif()
  find_package(Qt5 COMPONENTS Core REQUIRED)
  target_link_libraries(H5Support INTERFACE Qt5::Core)
endif()


# --------------------------------------------------------------------
# Setup the Exported Targets
# export(TARGETS H5Support
#   NAMESPACE H5Support::
#   FILE ${H5Support_BINARY_DIR}/H5SupportTargets.cmake
# )

# --------------------------------------------------------------------
# Setup the install rules for the various platforms
set(install_dir "bin")
set(lib_install_dir "lib")
set(ConfigPackageLocation share/H5Support)

if(WIN32)
  set(install_dir ".")
  set(lib_install_dir ".")
endif()


install(TARGETS ${PROJECT_NAME}
  COMPONENT Applications
  EXPORT ${PROJECT_NAME}Targets
  RUNTIME DESTINATION ${install_dir}
  LIBRARY DESTINATION ${lib_install_dir}
  ARCHIVE DESTINATION lib
)

# --------------------------------------------------------------------
# Allow the generation and installation of a CMake configuration file
# which makes using H5Support from another project easier.
# --------------------------------------------------------------------
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}TargetsConfigVersion.cmake"
  VERSION ${H5Support_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT ${PROJECT_NAME}Targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
  NAMESPACE H5Support::
)

configure_file(${H5Support_SOURCE_DIR}/cmake/H5SupportConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/H5SupportConfig.cmake"
  @ONLY
)

if(NOT H5Support_SKIP_INSTALL_FILES)
  install(
    FILES
      ${H5Support_HDRS}
    DESTINATION
      include/H5Support
    COMPONENT
      Devel
  )

  install(EXPORT ${PROJECT_NAME}Targets
    FILE
      ${PROJECT_NAME}Targets.cmake
    NAMESPACE
      H5Support::
    DESTINATION
      ${ConfigPackageLocation}
  )

  install(
    FILES
      "${CMAKE_CURRENT_BINARY_DIR}/H5SupportConfig.cmake"
      "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}TargetsConfigVersion.cmake"
    DESTINATION
      ${ConfigPackageLocation}
    COMPONENT
      Devel
  )

  # Install the LICENSE.txt as copyright to make VCPKG happy
  install(FILES ${H5Support_SOURCE_DIR}/LICENSE.txt DESTINATION ${ConfigPackageLocation} RENAME copyright)
endif()


# --------------------------------------------------------------------
# Setup unit testing
option(H5Support_BUILD_TESTING "Build H5Support tests" ON)

if(H5Support_BUILD_TESTING)
  include(${H5Support_SOURCE_DIR}/Test/CMakeLists.txt)
endif()
