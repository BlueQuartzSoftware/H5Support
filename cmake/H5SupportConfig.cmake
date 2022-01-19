#-------------------------------------------------------------------------------
# Find the Qt5 Library as we need that.
#-------------------------------------------------------------------------------
if(@H5Support_INCLUDE_QT_API@)
  set(Qt5_COMPONENTS "Core")
  # On Linux we need the DBus library
  if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(Qt5_COMPONENTS ${Qt5_COMPONENTS} DBus)
  endif()

  find_package(Qt5 COMPONENTS ${Qt5_COMPONENTS})
  if(NOT Qt5_FOUND)
    message(FATAL_ERROR "Qt5 is Required for H5Support to build. Please install it or set the Qt5_DIR cmake variable")
  endif()
  message(STATUS "* Qt5 (${Qt5_VERSION}) ${Qt5_DIR}")
endif()

include(CMakeFindDependencyMacro)
find_dependency(HDF5 NAMES hdf5 REQUIRED)

include("${CMAKE_CURRENT_LIST_DIR}/H5SupportTargets.cmake")

set(H5Support_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../include")
set(H5Support_LIB_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../lib;${CMAKE_CURRENT_LIST_DIR}/../../bin")
