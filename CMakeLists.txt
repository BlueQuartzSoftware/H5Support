cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0077 NEW)

cmake_minimum_required(VERSION 3.18.0)

PROJECT (H5StudioProj
  VERSION 0.1.0
  DESCRIPTION "Open Source Viewer of HDF5 Data"
  LANGUAGES CXX
)

# ---------- Setup output Directories -------------------------
if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${H5StudioProj_BINARY_DIR}/Bin
    CACHE PATH
    "Single Directory for all Libraries"
  )
endif()

# --------- Setup the Executable output Directory -------------
if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${H5StudioProj_BINARY_DIR}/Bin
    CACHE PATH
    "Single Directory for all Executables."
  )
endif()

# --------- Setup the Executable output Directory -------------
if(NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${H5StudioProj_BINARY_DIR}/Bin
    CACHE PATH
    "Single Directory for all static libraries."
  )
endif()


list(APPEND CMAKE_MODULE_PATH ${H5StudioProj_SOURCE_DIR}/cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------------------------
# Find Qt5 Widgets
# ------------------------------------------------------------------------------
include(${H5StudioProj_SOURCE_DIR}/cmake/Qt5Support.cmake)
set(H5StudioProj_Qt5_Components Core Widgets Network Gui Concurrent OpenGL PrintSupport)
CMP_AddQt5Support("${H5StudioProj_Qt5_Components}"
  ""
  "${H5StudioProj_BINARY_DIR}"
  "DREAM3DNXProj")

set(AUTOMOC_SOURCE_GROUP "Generated/moc")

# ------------------------------------------------------------------------------
# Find HDF5
# ------------------------------------------------------------------------------
find_package(HDF5 REQUIRED)

add_subdirectory(${H5StudioProj_SOURCE_DIR}/3rdParty/H5Support)

add_subdirectory(${H5StudioProj_SOURCE_DIR}/src/H5Studio)
