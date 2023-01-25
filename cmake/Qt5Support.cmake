# --------------------------------------------------------------------
#-- Copy all the Qt5 dependent DLLs into the current build directory so that
#-- one can debug an application or library that depends on Qt5 libraries.
#-- This macro is really intended for Windows Builds because windows libraries
#-- do not have any type of rpath or install_name encoded in the libraries so
#-- the least intrusive way to deal with the PATH issues is to just copy all
#-- the dependend DLL libraries into the build directory. Note that this is
#-- NOT needed for static libraries.
function(CopyQt5RunTimeLibraries)

  set(options)
  set(oneValueArgs PREFIX)
  set(multiValueArgs LIBRARIES)

  cmake_parse_arguments(P "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
  # message(STATUS "Copying Qt5 Runtime Libraries: ${P_LIBRARIES}")
  set(SUPPORT_LIB_OPTION 1)
  if(MSVC_IDE)
    set(SUPPORT_LIB_OPTION 0)
  elseif(APPLE) # Apple systems do NOT need this so just skip this entirely
    return()
  elseif(UNIX AND NOT MSVC)
    set(SUPPORT_LIB_OPTION 3)
  endif()

  get_filename_component(QT_DLL_PATH_tmp ${QtQMake_location} PATH)

  if(SUPPORT_LIB_OPTION EQUAL 0)
    # message(STATUS "SUPPORT_LIB_OPTION = 0")

    foreach(qtlib ${P_LIBRARIES})
      set(TYPE "d")
      # message(STATUS "Copy Rule for Qt library ${P_PREFIX}${qtlib}${TYPE}.dll")
      # We need to copy both the Debug and Release versions of the libraries into their respective
      # subfolders for Visual Studio builds
      if(NOT TARGET ZZ_${P_PREFIX}${qtlib}${TYPE}-Debug-Copy)
        add_custom_target(ZZ_${P_PREFIX}${qtlib}${TYPE}-Debug-Copy ALL
                            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH_tmp}/${P_PREFIX}${qtlib}${TYPE}.dll
                            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/
                            # COMMENT "Copying ${P_PREFIX}${qtlib}${TYPE}.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/"
                            )
        set_target_properties(ZZ_${P_PREFIX}${qtlib}${TYPE}-Debug-Copy PROPERTIES FOLDER ZZ_COPY_FILES/Debug/Qt5)
        get_property(COPY_LIBRARY_TARGETS GLOBAL PROPERTY COPY_LIBRARY_TARGETS)
        set_property(GLOBAL PROPERTY COPY_LIBRARY_TARGETS ${COPY_LIBRARY_TARGETS} ZZ_${P_PREFIX}${qtlib}${TYPE}-Debug-Copy)
      endif()

    #   message(STATUS "Generating Copy Rule for Qt Release DLL Library ${QT_DLL_PATH_tmp}/${qtlib}d.dll")
      set(TYPE "")
      if(NOT TARGET ZZ_${P_PREFIX}${qtlib}${TYPE}-Release-Copy)
        add_custom_target(ZZ_${P_PREFIX}${qtlib}${TYPE}-Release-Copy ALL
                            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH_tmp}/${P_PREFIX}${qtlib}.dll
                            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/
                            # COMMENT "Copying ${P_PREFIX}${qtlib}.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/"
                            )
        set_target_properties(ZZ_${P_PREFIX}${qtlib}${TYPE}-Release-Copy PROPERTIES FOLDER ZZ_COPY_FILES/Release/Qt5)
        get_property(COPY_LIBRARY_TARGETS GLOBAL PROPERTY COPY_LIBRARY_TARGETS)
        set_property(GLOBAL PROPERTY COPY_LIBRARY_TARGETS ${COPY_LIBRARY_TARGETS} ZZ_${P_PREFIX}${qtlib}${TYPE}-Release-Copy)
      endif()
    endforeach(qtlib)
  elseif(SUPPORT_LIB_OPTION EQUAL 1)
    # This will get hit if Ninja, MinGW, MSYS, Cygwin is used for the build system
    # message(STATUS "SUPPORT_LIB_OPTION = 1")

    foreach(qtlib ${P_LIBRARIES})
      set(TYPE "")
      if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
          set(TYPE "d")
      else()

      endif()

      # message(STATUS "Copy Rule for Qt library ${P_PREFIX}${qtlib}${TYPE}.dll")
      # We need to copy the library into the "Bin" Folder
      if(NOT TARGET ZZ_${qtlib}-Copy)
        add_custom_target(ZZ_${qtlib}-Copy ALL
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH_tmp}/${P_PREFIX}${qtlib}${TYPE}.dll
                            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
                            COMMENT "Copying ${P_PREFIX}${qtlib}${TYPE}.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")
        set_target_properties(ZZ_${qtlib}-Copy PROPERTIES FOLDER ZZ_COPY_FILES/Qt5)
        get_property(COPY_LIBRARY_TARGETS GLOBAL PROPERTY COPY_LIBRARY_TARGETS)
        set_property(GLOBAL PROPERTY COPY_LIBRARY_TARGETS ${COPY_LIBRARY_TARGETS} ZZ_${qtlib}-Copy)
      endif()
    endforeach(qtlib)
  endif()
endfunction()


# -------------------------------------------------------------
# This function adds the necessary cmake code to find the Qt5
# shared libraries and setup custom copy commands and/or install
# rules for Linux and Windows to use
function(AddQt5SupportLibraryCopyInstallRules)
  set(options )
  set(oneValueArgs PREFIX DEBUG_SUFFIX)
  set(multiValueArgs LIBRARIES)
  cmake_parse_arguments(P "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  set(SUPPORT_LIB_OPTION 1)
  if(MSVC_IDE)
    set(SUPPORT_LIB_OPTION 0)
  elseif(APPLE) # Apple systems do NOT need this so just skip this entirely
   return()
  elseif(UNIX AND NOT MSVC)
    set(SUPPORT_LIB_OPTION 3)
  endif()

  get_filename_component(QT_DLL_PATH ${QtQMake_location} PATH)

  if(WIN32)
    set(destination "./")
  else()
    set(destination "lib")
  endif()

  if(SUPPORT_LIB_OPTION EQUAL 0)
    # We need to copy both the Debug and Release versions of the libraries into their respective
    # subfolders for Visual Studio builds
    set(OUTPUT_DIRS "Release;Debug;MinSizeRel;RelWithDebInfo")
    foreach(qtlib ${P_LIBRARIES})
      foreach(INT_DIR ${OUTPUT_DIRS})
        set(SUFFIX "")
        if("${INT_DIR}" STREQUAL "Debug")
          set(SUFFIX ${P_DEBUG_SUFFIX})
        endif()
        # message(STATUS "Copy Rule for Qt Support library ${qtlib}${SUFFIX}.dll")
        if(NOT TARGET ZZ_${qtlib}-${INT_DIR}-Copy)
          add_custom_target(ZZ_${qtlib}-${INT_DIR}-Copy ALL
                              COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INT_DIR}
                              COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH}/${P_PREIX}${qtlib}${SUFFIX}.dll
                              ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INT_DIR}/
                              COMMENT "Copying ${P_PREIX}${qtlib}${SUFFIX}.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INT_DIR}/")
          set_target_properties(ZZ_${qtlib}-${INT_DIR}-Copy PROPERTIES FOLDER ZZ_COPY_FILES/${INT_DIR}/Qt5)
          install(FILES ${QT_DLL_PATH}/${P_PREIX}${qtlib}${SUFFIX}.dll  DESTINATION "${destination}" CONFIGURATIONS ${INT_DIR} COMPONENT Applications)
          get_property(COPY_LIBRARY_TARGETS GLOBAL PROPERTY COPY_LIBRARY_TARGETS)
          set_property(GLOBAL PROPERTY COPY_LIBRARY_TARGETS ${COPY_LIBRARY_TARGETS} ZZ_${qtlib}-${INT_DIR}-Copy)
        endif()
      endforeach()
    endforeach(qtlib)

  elseif(SUPPORT_LIB_OPTION EQUAL 1)
  # This should be the code path for Ninja/NMake/Makefiles all on NON-OS X systems
    set(SUFFIX "")
    if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(SUFFIX ${P_DEBUG_SUFFIX})
    endif()
    set(INT_DIR "")
    foreach(qtlib ${P_LIBRARIES})
      # message(STATUS "Copy Rule for Qt Support library ${P_PREIX}${qtlib}${SUFFIX}.dll")
      # We need to copy the library into the "Bin" folder
      if(NOT TARGET ZZ_${qtlib}-Copy)
        add_custom_target(ZZ_${qtlib}-Copy ALL
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_DLL_PATH}/${P_PREIX}${qtlib}${SUFFIX}.dll
                            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
                            COMMENT "Copying ${P_PREIX}${qtlib}${SUFFIX}.dll to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
        set_target_properties(ZZ_${qtlib}-Copy PROPERTIES FOLDER ZZ_COPY_FILES/${INT_DIR}/Qt5)
        install(FILES ${QT_DLL_PATH}/${P_PREIX}${qtlib}${SUFFIX}.dll  DESTINATION "${destination}" CONFIGURATIONS ${CMAKE_BUILD_TYPE} COMPONENT Applications)
        get_property(COPY_LIBRARY_TARGETS GLOBAL PROPERTY COPY_LIBRARY_TARGETS)
        set_property(GLOBAL PROPERTY COPY_LIBRARY_TARGETS ${COPY_LIBRARY_TARGETS} ZZ_${qtlib}-Copy)        
      endif()
    endforeach(qtlib)
  endif()
endfunction()

# --------------------------------------------------------------------
# Creates the appropriate qt.conf files for our build. Apple systems
# are ignored as the necessary rules are taken care of during the
# packaging phases.
# --------------------------------------------------------------------
function(AddQtConfTargets)
  set(options)
  set(oneValueArgs QT_PLUGINS_DIR QT_QML_DIR)
  set(multiValueArgs)

  cmake_parse_arguments(P "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  # For macOS systems, we don't need any of this so return now.
  if(APPLE)
    return()
  endif()
  
  # Create a QtConf file in all locations for each kind of build under Visual Studio
  if(MSVC_IDE)
    set(OUTPUT_DIRS "Release;Debug;MinSizeRel;RelWithDebInfo")
    foreach(INT_DIR ${OUTPUT_DIRS})
      set(qt_conf_file "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INT_DIR}/qt.conf")
      # Start Writing the qt.conf file
      file(WRITE ${qt_conf_file} "[Paths]\nPrefix = .\n")
      file(APPEND ${qt_conf_file} "LibraryExecutables = .\n")
      file(APPEND ${qt_conf_file} "Data = .\n")
      file(APPEND ${qt_conf_file} "Plugins = ${P_QT_PLUGINS_DIR}\n")
      file(APPEND ${qt_conf_file} "Qml2Imports = ${P_QT_QML_DIR}\n")
    endforeach()
  elseif() # Create a qt.conf file for every other kind of Generator
    set(INT_DIR "")
    set(qt_conf_file "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${INT_DIR}/qt.conf")
    # Create the qt.conf file so that the image plugins will be loaded correctly
    file(WRITE ${qt_conf_file} "[Paths]\nPrefix = .\n")
    file(APPEND ${qt_conf_file} "LibraryExecutables = .\n")
    file(APPEND ${qt_conf_file} "Data = .\n")
    file(APPEND ${qt_conf_file} "Plugins = ${P_QT_PLUGINS_DIR}\n")
    file(APPEND ${qt_conf_file} "Qml2Imports = ${P_QT_QML_DIR}\n")
  endif()

  set(QTCONF_DIR "bin")
  set(QTPLUGINS_DIR "../")
  if(WIN32)
    set(QTCONF_DIR ".")
    set(QTPLUGINS_DIR "")
  endif()
  # Create an Installation rule for MSVC and Linux.
  set(qt_conf_file "${PROJECT_BINARY_DIR}/qt.conf")
  file(WRITE ${qt_conf_file} "[Paths]\nPrefix = .\n")
  file(APPEND ${qt_conf_file} "LibraryExecutables = .\n")
  file(APPEND ${qt_conf_file} "Data = .\n")
  file(APPEND ${qt_conf_file} "Plugins = ${QTPLUGINS_DIR}Plugins\n")
  file(APPEND ${qt_conf_file} "Qml2Imports = qml\n")
  install(FILES ${qt_conf_file}
    DESTINATION ${QTCONF_DIR}
    COMPONENT Applications)

endfunction()

# --------------------------------------------------------------------
#
#
# --------------------------------------------------------------------
function(AddQt5LibraryInstallRule)
  set(options)
  set(oneValueArgs)
  set(multiValueArgs LIBRARIES)

  cmake_parse_arguments(P "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
  # message(STATUS "Install Rules Qt5 Libraries: ${P_LIBRARIES}")
  if(APPLE)
    return()
  endif()

  set(build_types "Debug;Release")
  if(WIN32)
    set(qt_plugin_prefix "")
    set(qt_plugin_DEBUG_suffix "d")
    set(qt_plugin_RELEASE_suffix "")
    set(destination "./")
  else()
    set(qt_plugin_prefix "lib")
    set(qt_plugin_DEBUG_suffix "_debug")
    set(qt_plugin_RELEASE_suffix "")
    set(destination "lib")
  endif()

  foreach(qtlib ${P_LIBRARIES})
    foreach(build_type ${build_types})
      string(TOUPPER ${build_type} UpperBType)
      get_target_property(dll_lib_path Qt5::${qtlib} IMPORTED_LOCATION_${UpperBType})
      if(NOT "${dll_lib_path}" STREQUAL "dll_lib_path-NOTFOUND")
        # message(STATUS "  [${qtlib}-${build_type}]: Creating Install Rule")
        install(FILES ${dll_lib_path}
                DESTINATION "${destination}"
                CONFIGURATIONS ${build_type}
                COMPONENT Applications)
      endif()
    endforeach()
  endforeach()

endfunction()

#-------------------------------------------------------------------------------
# Finds plugins from the Qt installation. The pluginlist argument should be
# something like "QDDS QGif QICNS QICO QJp2 QJpeg QMng QTga QTiff QWbmp QWebp"
#-------------------------------------------------------------------------------
function(AddQt5Plugins)
  set(options)
  set(oneValueArgs PLUGIN_FILE PLUGIN_FILE_TEMPLATE PLUGIN_TYPE PLUGIN_SUFFIX)
  set(multiValueArgs PLUGIN_NAMES)
  if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release")
  endif()
  cmake_parse_arguments(P "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  if(APPLE)
    return()
  endif()

  set(build_types "Debug;Release")
  if(WIN32)
    set(qt_plugin_prefix "")
    set(qt_plugin_DEBUG_suffix "d")
    set(qt_plugin_RELEASE_suffix "")
  else()
    set(qt_plugin_prefix "lib")
    set(qt_plugin_DEBUG_suffix "_debug")
    set(qt_plugin_RELEASE_suffix "")
  endif()

  # message(STATUS "Qt5 Plugins: ${P_PLUGIN_NAMES}")
  # We only use install rules for Linux/Windows.
  foreach(build_type ${build_types})
    # message(STATUS "build_type: ${build_type}")
    foreach(plugin ${P_PLUGIN_NAMES})
      if(TARGET Qt5::${plugin}${P_PLUGIN_SUFFIX})
        get_target_property(${build_type}_loc Qt5::${plugin}${P_PLUGIN_SUFFIX} LOCATION_${build_type})

          install(FILES ${${build_type}_loc}
              DESTINATION ./Plugins/${P_PLUGIN_TYPE}
              CONFIGURATIONS ${build_type}
              COMPONENT Applications)
        endif()

    endforeach()
  endforeach()

endfunction()

# ------------------------------------------------------------------------------
# Macro CMP_AddQt5Support
# Qt 5 Section: This section is the base cmake code that will find Qt5 on the computer and
# setup all the necessary Qt5 modules that need to be used.
#  @param Qt5Components These are the Qt Components that the project needs. The
#    possible values are: Core Widgets Network Gui Concurrent Script Svg Xml OpenGL PrintSupport
#    Note that one OR more components can be selected.
#  @param ProjectBinaryDir The Directory where to write any output files
# ------------------------------------------------------------------------------
macro(CMP_AddQt5Support Qt5Components Qt5QmlComponents ProjectBinaryDir VarPrefix)

  # ------------------------------------------------------------------------------
  # Find includes in corresponding build directories
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  # Find the QtWidgets library
  set(Qt5_COMPONENTS "${Qt5Components}")
  set(Qml_COMPONENTS "${Qt5QmlComponents}")

  # This line gets the list of Qt5 components into the calling context.
  set(${VarPrefix}_Qt5_Components ${Qt5_COMPONENTS} CACHE STRING "" FORCE)
  set(${VarPrefix}_Qml_COMPONENTS ${Qml_COMPONENTS} CACHE STRING "" FORCE)

  # On Linux we need the DBus library
  if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(Qt5_COMPONENTS ${Qt5_COMPONENTS} DBus)
  endif()

  find_package(Qt5 COMPONENTS ${Qt5_COMPONENTS})
 # find_package(Qt5Quick CONFIG REQUIRED Private)
 # find_package(Qt5QuickCompiler)
  set(CMAKE_AUTOMOC ON)
  # set(CMAKE_AUTOUIC ON)
  # set(CMAKE_AUTORCC ON)

  if(NOT Qt5_FOUND)
    message(FATAL_ERROR "Qt5 is Required for ${PROJECT_NAME} to build. Please install it.")
  endif()

  # add_compile_definitions(QT_NO_KEYWORDS)

  # We need the location of QMake for later on in order to find the plugins directory
  get_target_property(QtQMake_location Qt5::qmake LOCATION)
  get_property(Qt5_STATUS_PRINTED GLOBAL PROPERTY Qt5_STATUS_PRINTED)
  execute_process(COMMAND "${QtQMake_location}" -query QT_INSTALL_PREFIX OUTPUT_VARIABLE QM_QT_INSTALL_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND "${QtQMake_location}" -query QT_VERSION OUTPUT_VARIABLE QM_QT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND "${QtQMake_location}" -query QT_INSTALL_PLUGINS OUTPUT_VARIABLE QM_QT_INSTALL_PLUGINS OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND "${QtQMake_location}" -query QT_INSTALL_QML OUTPUT_VARIABLE QM_QT_INSTALL_QML OUTPUT_STRIP_TRAILING_WHITESPACE)

  # Need this variable set here for use later on
  set(QT_ROOT "${QM_QT_INSTALL_PREFIX}")
  if(NOT Qt5_STATUS_PRINTED)
    message(STATUS "Qt5 Location: ${QM_QT_INSTALL_PREFIX}")
    message(STATUS "Qt5 Version: ${QM_QT_VERSION} ")
    set_property(GLOBAL PROPERTY Qt5_STATUS_PRINTED TRUE)
  endif()

  if(NOT DEFINED CMP_QT5_ENABLE_INSTALL)
    set(CMP_QT5_ENABLE_INSTALL ON)
  endif()

  if(NOT DEFINED CMP_QT5_ENABLE_COPY)
    set(CMP_QT5_ENABLE_COPY ON)
  endif()

  # This is really just needed for Windows
  if(CMP_QT5_ENABLE_COPY)
    # CopyQt5RunTimeLibraries(LIBRARIES ${Qt5_COMPONENTS} PREFIX Qt5)
  endif()

  if(CMP_QT5_ENABLE_INSTALL)
    # This is pretty much needed on all the platforms.
    AddQt5LibraryInstallRule(LIBRARIES ${Qt5_COMPONENTS})
  endif()

  set(Qt5_ICU_COMPONENTS "")
  if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(Qt5_ICU_COMPONENTS icui18n icuuc icudata)
  endif()
  # Each Platform has a set of support libraries that need to be copied
  AddQt5SupportLibraryCopyInstallRules( LIBRARIES ${Qt5_ICU_COMPONENTS} PREFIX "" DEBUG_SUFFIX "")

  set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "Qt5AutoMocTargets")

  get_property(QT_PLUGINS_FILE GLOBAL PROPERTY QtPluginsTxtFile)
  if("${QT_PLUGINS_FILE}" STREQUAL "")
    set_property(GLOBAL PROPERTY QtPluginsTxtFile "${ProjectBinaryDir}/Qt_Plugins.txt")
    get_property(QT_PLUGINS_FILE GLOBAL PROPERTY QtPluginsTxtFile)
  endif()

  get_property(QT_PLUGINS_FILE_TEMPLATE GLOBAL PROPERTY QtPluginsCMakeFile)
  if("${QT_PLUGINS_FILE_TEMPLATE}" STREQUAL "")
    set_property(GLOBAL PROPERTY QtPluginsCMakeFile "${ProjectBinaryDir}/Qt_Plugins.cmake.in")
    get_property(QT_PLUGINS_FILE_TEMPLATE GLOBAL PROPERTY QtPluginsCMakeFile)
  endif()

  if(NOT DREAM3DNXProj_ANACONDA)
    AddQtConfTargets(QT_PLUGINS_DIR ${QM_QT_INSTALL_PLUGINS} QT_QML_DIR ${QM_QT_INSTALL_QML})
  endif()

  file(WRITE ${QT_PLUGINS_FILE_TEMPLATE} "")
  file(WRITE ${QT_PLUGINS_FILE} "")

  if(CMP_QT5_ENABLE_INSTALL)
    list(FIND Qt5_COMPONENTS "Gui" NeedsGui)
    if(NeedsGui GREATER -1)
      AddQt5Plugins(PLUGIN_NAMES QDDS QGif QICNS QICO QJp2 QJpeg QMng QTga QTiff QWbmp QWebp
                  PLUGIN_FILE "${QT_PLUGINS_FILE}"
                  PLUGIN_FILE_TEMPLATE "${QT_PLUGINS_FILE_TEMPLATE}"
                  PLUGIN_SUFFIX Plugin
                  PLUGIN_TYPE imageformats)
      if(WIN32)
        AddQt5Plugins(PLUGIN_NAMES QWindowsIntegration
                    PLUGIN_FILE "${QT_PLUGINS_FILE}"
                    PLUGIN_FILE_TEMPLATE "${QT_PLUGINS_FILE_TEMPLATE}"
                    PLUGIN_SUFFIX Plugin
                    PLUGIN_TYPE platforms)
        AddQt5Plugins(PLUGIN_NAMES QWindowsDirect2DIntegration
                    PLUGIN_FILE "${QT_PLUGINS_FILE}"
                    PLUGIN_FILE_TEMPLATE "${QT_PLUGINS_FILE_TEMPLATE}"
                    PLUGIN_SUFFIX Plugin
                    PLUGIN_TYPE platforms)
        AddQt5Plugins(PLUGIN_NAMES QMinimalIntegration
                    PLUGIN_FILE "${QT_PLUGINS_FILE}"
                    PLUGIN_FILE_TEMPLATE "${QT_PLUGINS_FILE_TEMPLATE}"
                    PLUGIN_SUFFIX Plugin
                    PLUGIN_TYPE platforms)
        AddQt5Plugins(PLUGIN_NAMES QOffscreenIntegration
                    PLUGIN_FILE "${QT_PLUGINS_FILE}"
                    PLUGIN_FILE_TEMPLATE "${QT_PLUGINS_FILE_TEMPLATE}"
                    PLUGIN_SUFFIX Plugin
                    PLUGIN_TYPE platforms)

        AddQt5Plugins(PLUGIN_NAMES QWindowsVistaStyle
                    PLUGIN_FILE "${QT_PLUGINS_FILE}"
                    PLUGIN_FILE_TEMPLATE "${QT_PLUGINS_FILE_TEMPLATE}"
                    PLUGIN_SUFFIX Plugin
                    PLUGIN_TYPE styles)
      endif()

      if(CMAKE_SYSTEM_NAME MATCHES "Linux")
        AddQt5Plugins(PLUGIN_NAMES QXcbIntegration
                    PLUGIN_FILE "${QT_PLUGINS_FILE}"
                    PLUGIN_FILE_TEMPLATE "${QT_PLUGINS_FILE_TEMPLATE}"
                    PLUGIN_SUFFIX Plugin
                    PLUGIN_TYPE platforms)
      endif()
    endif()

    list(FIND Qt5_COMPONENTS "Network" NeedsNetwork)
    if(NeedsNetwork GREATER -1)
      AddQt5Plugins(PLUGIN_NAMES QGenericEnginePlugin
                    PLUGIN_FILE "${QT_PLUGINS_FILE}"
                    PLUGIN_FILE_TEMPLATE "${QT_PLUGINS_FILE_TEMPLATE}"
                    # PLUGIN_SUFFIX Plugin
                    PLUGIN_TYPE bearer)
    endif()

    list(FIND Qt5_COMPONENTS "Sql" NeedsSql)
    if(NeedsSql GREATER -1)
      AddQt5Plugins(PLUGIN_NAMES QODBCDriverPlugin
                    PLUGIN_FILE "${QT_PLUGINS_FILE}"
                    PLUGIN_FILE_TEMPLATE "${QT_PLUGINS_FILE_TEMPLATE}"
                    # PLUGIN_SUFFIX Plugin
                    PLUGIN_TYPE sqldrivers)
    endif()

    list(FIND Qt5_COMPONENTS "Multimedia" NeedsMultiMedia)
    if(NeedsMultiMedia GREATER -1)
      if(WIN32)
        AddQt5Plugins(PLUGIN_NAMES AudioCaptureService DSService WMFService
          PLUGIN_SUFFIX Plugin
          PLUGIN_TYPE mediaservice
        )
      endif()
    endif()

    if(CMP_ENABLE_PRINTSUPPORT_PLUGIN)
      list(FIND Qt5_COMPONENTS "PrintSupport" NeedsPrintSupport)
      if(NeedsPrintSupport GREATER -1)
        if(WIN32)
          AddQt5Plugins(PLUGIN_NAMES QWindowsPrinterSupport
            PLUGIN_SUFFIX Plugin
            PLUGIN_TYPE printsupport
          )
        endif()
      endif()
    endif()

    if(0)
      AddQt5Plugins(PLUGIN_NAMES AccessibleFactory
                  PLUGIN_FILE "${QT_PLUGINS_FILE}"
                  PLUGIN_FILE_TEMPLATE "${QT_PLUGINS_FILE_TEMPLATE}"
                  PLUGIN_TYPE accessible)
    endif()
  endif()

  # Append the locations of the Qt libraries to our Library Search Paths
  list(APPEND CMP_LIB_SEARCH_DIRS ${QT_BINARY_DIR} ${QT_LIBRARY_DIR} )

endmacro()

