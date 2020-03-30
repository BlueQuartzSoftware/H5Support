
function(AddH5SupportSources)
  
  set(options )
  set(oneValueArgs TARGET QT_SUPPORT BINARY_DIR)
  set(multiValueArgs )
  cmake_parse_arguments(Z "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  set(H5Support_USE_QT ${Z_QT_SUPPORT})

  set(H5Support_BINARY_DIR ${Z_BINARY_DIR})
  if("${H5Support_SOURCE_DIR}" STREQUAL "")
    message(FATAL_ERROR "H5Support_SOURCE_DIR was not set in CMake Cache. That is a severe problem. Contact the developers")
  endif()
  
  include(${H5Support_SOURCE_DIR}/Resources/CMake/TargetCopyInstall.cmake)

  set(H5Support_HDRS
    ${H5Support_SOURCE_DIR}/Source/H5Support/H5Lite.h
    ${H5Support_SOURCE_DIR}/Source/H5Support/H5Utilities.h
    ${H5Support_SOURCE_DIR}/Source/H5Support/H5ScopedSentinel.h
    ${H5Support_SOURCE_DIR}/Source/H5Support/H5ScopedErrorHandler.h
    ${H5Support_SOURCE_DIR}/Source/H5Support/H5Macros.h
  )

  set(H5Support_SRCS
    ${H5Support_SOURCE_DIR}/Source/H5Support/H5Lite.cpp
    ${H5Support_SOURCE_DIR}/Source/H5Support/H5Utilities.cpp
    ${H5Support_SOURCE_DIR}/Source/H5Support/H5ScopedSentinel.cpp
    ${H5Support_SOURCE_DIR}/Source/H5Support/H5ScopedErrorHandler.cpp
  )

  if(H5Support_USE_QT)
    list(APPEND H5Support_HDRS
      ${H5Support_SOURCE_DIR}/Source/H5Support/QH5Lite.h
      ${H5Support_SOURCE_DIR}/Source/H5Support/QH5Utilities.h
    )  

    list(APPEND H5Support_SRCS
      ${H5Support_SOURCE_DIR}/Source/H5Support/QH5Lite.cpp
      ${H5Support_SOURCE_DIR}/Source/H5Support/QH5Utilities.cpp
    )
  endif()

  # These are generated source/header files
  set(H5Support_VERSION_CPP_PATH ${H5Support_BINARY_DIR}/H5Support/H5SupportVersion.cpp)
  set(H5Support_VERSION_H_PATH ${H5Support_BINARY_DIR}/H5Support/H5SupportVersion.h)

  set(VERSION_EXPORT_HEADER_INCLUDE "\"H5Support/H5Support.h\"")
  set(VERSION_EXPORT "H5Support_EXPORT")
  set(VERSION_NAMESPACE "H5Support")
  set(VERSION_HEADER_NAME "H5SupportVersion.h")
  set(PROJECT_NAME "H5Support")
  set(INCLUDE_H5SUPPORT_DLL_HEADER "#define H5Support_EXPORT")


  configure_file(${H5Support_SOURCE_DIR}/Resources/CMake/Version.cpp.in ${H5Support_VERSION_CPP_PATH})
  configure_file(${H5Support_SOURCE_DIR}/Resources/CMake/Version.h.in ${H5Support_VERSION_H_PATH})

  set(H5Support_CONFIG_HEADER ${H5Support_BINARY_DIR}/H5Support/H5Support.h)
  configure_file(${H5Support_SOURCE_DIR}/Resources/CMake/H5Support.h.in ${H5Support_CONFIG_HEADER})

  set(H5Support_GENERATED_HEADERS
    ${H5Support_VERSION_H_PATH}
    ${H5Support_CONFIG_HEADER}
    ${H5Support_EXPORT_FILE_NAME}
  )

  set(H5Support_GENERATED_SOURCES
    ${H5Support_VERSION_CPP_PATH}
  )

  target_include_directories(${Z_TARGET}
    PUBLIC
      $<INSTALL_INTERFACE:include>
      $<BUILD_INTERFACE:${H5Support_SOURCE_DIR}/Source>
      $<BUILD_INTERFACE:${H5Support_BINARY_DIR}>
  )


  target_sources(${Z_TARGET} PRIVATE 
            ${H5Support_HDRS}
            ${H5Support_SRCS}
            ${H5Support_GENERATED_HEADERS}
            ${H5Support_GENERATED_SOURCES}  
  )
  source_group("H5Support" FILES ${H5Support_HDRS} ${H5Support_SRCS})
  source_group("Generated" FILES ${H5Support_GENERATED_HEADERS} ${H5Support_GENERATED_SOURCES})

  # If we compiled against HDF5, we need that on the list of link libs
  target_compile_definitions(${Z_TARGET} PRIVATE -DEbsdLib_HAVE_HDF5)
  

endfunction()
