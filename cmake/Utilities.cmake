#-------------------------------------------------------------------------------
# 
#-------------------------------------------------------------------------------
function(enable_vcpkg_manifest_feature)
  set(optionsArgs)
  set(oneValueArgs TEST_VAR FEATURE)
  set(multiValueArgs)
  cmake_parse_arguments(ARGS "${optionsArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(${ARGS_TEST_VAR})
    if(NOT ${ARGS_FEATURE} IN_LIST VCPKG_MANIFEST_FEATURES)
      set(VCPKG_MANIFEST_FEATURES ${VCPKG_MANIFEST_FEATURES} ${ARGS_FEATURE} PARENT_SCOPE)
    endif()
  endif()
endfunction()

#-------------------------------------------------------------------------------
# 
#-------------------------------------------------------------------------------
function(install_with_directory)
  set(optionsArgs)
  set(oneValueArgs DESTINATION COMPONENT BASE_DIR)
  set(multiValueArgs FILES)
  cmake_parse_arguments(ARGS "${optionsArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  foreach(file ${ARGS_FILES})
      file(RELATIVE_PATH relative_file ${ARGS_BASE_DIR} ${file})
      get_filename_component(destination_dir ${relative_file} DIRECTORY)
      install(FILES ${file}
        DESTINATION ${ARGS_DESTINATION}/${destination_dir}
        COMPONENT ${ARGS_COMPONENT}
      )
  endforeach()
endfunction()


#-------------------------------------------------------------------------------
# This function generates a file ONLY if the MD5 between the "to be" generated file
# and the current file are different. This will help reduce recompiles based on
# the generation of files that are really the same.
#-------------------------------------------------------------------------------
function(cmpConfigureFileWithMD5Check)
    set(options)
    set(oneValueArgs CONFIGURED_TEMPLATE_PATH GENERATED_FILE_PATH )
    cmake_parse_arguments(GVS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

 #   message(STATUS "   GVS_CONFIGURED_TEMPLATE_PATH: ${GVS_CONFIGURED_TEMPLATE_PATH}")
 #   message(STATUS "   GVS_GENERATED_FILE_PATH: ${GVS_GENERATED_FILE_PATH}")

    # Only Generate a file if it is different than what is already there.
    if(EXISTS ${GVS_GENERATED_FILE_PATH} )
        file(MD5 ${GVS_GENERATED_FILE_PATH} VERSION_HDR_MD5)
        configure_file(${GVS_CONFIGURED_TEMPLATE_PATH}   ${GVS_GENERATED_FILE_PATH}_tmp  )

        file(MD5 ${GVS_GENERATED_FILE_PATH}_tmp VERSION_GEN_HDR_MD5)
        #message(STATUS "  File Exists, doing MD5 Comparison")

        # Compare the MD5 checksums. If they are different then configure the file into the proper location
        if(NOT "${VERSION_HDR_MD5}" STREQUAL "${VERSION_GEN_HDR_MD5}")
            #message(STATUS "   ${VERSION_GEN_HDR_MD5}")
            #message(STATUS "   ${VERSION_HDR_MD5}")
            #message(STATUS "  Files differ: Replacing with newly generated file")
            configure_file(${GVS_CONFIGURED_TEMPLATE_PATH}  ${GVS_GENERATED_FILE_PATH} )
        else()
            #message(STATUS "  NO Difference in Files")
        endif()
        file(REMOVE ${GVS_GENERATED_FILE_PATH}_tmp)
    else()
      # message(STATUS "  File does NOT Exist, Generating one...")
      configure_file(${GVS_CONFIGURED_TEMPLATE_PATH} ${GVS_GENERATED_FILE_PATH} )
    endif()

endfunction()


#-------------------------------------------------------------------------------
# We are going to use Git functionality to create a version number for our package
# The MAJOR.MINOR.PATCH is based off of YYYY.MM.DD
# The TWEAK is the git hash of project.
#-------------------------------------------------------------------------------
function(cmpBuildDateRevisionString)
  set(options)
  set(oneValueArgs GENERATED_HEADER_FILE_PATH GENERATED_SOURCE_FILE_PATH
                   NAMESPACE PROJECT_NAME EXPORT_MACRO VERSION_MACRO_PATH STRING_CLASS STRING_INCLUDE)
  cmake_parse_arguments(GVS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  if(NOT DEFINED GVS_STRING_CLASS)
    set(GVS_STRING_CLASS "QString")
  endif()

  if(NOT DEFINED GVS_STRING_INCLUDE)
    set(GVS_STRING_INCLUDE "<QtCore/QString>")
  endif()

  if(0)
    message(STATUS "--------------------------------------------")
    message(STATUS "GVS_NAMESPACE: ${GVS_NAMESPACE}")
    message(STATUS "GVS_PROJECT_NAME: ${GVS_PROJECT_NAME}")
    message(STATUS "GVS_GENERATED_HEADER_FILE_PATH: ${GVS_GENERATED_HEADER_FILE_PATH}")
    message(STATUS "GVS_GENERATED_SOURCE_FILE_PATH: ${GVS_GENERATED_SOURCE_FILE_PATH}")
    message(STATUS "GVS_PROJECT_SOURCE_DIR: ${GVS_PROJECT_SOURCE_DIR}")
    message(STATUS "GVS_PROJECT_VERSION_MAJOR: ${GVS_PROJECT_VERSION_MAJOR}")
    message(STATUS "GVS_EXPORT_MACRO: ${GVS_EXPORT_MACRO}")
    message(STATUS "${GVS_PROJECT_NAME}_BUILD_DATE: ${${GVS_PROJECT_NAME}_BUILD_DATE}")
    message(STATUS "${GVS_PROJECT_NAME}_SOURCE_DIR: ${${GVS_PROJECT_NAME}_SOURCE_DIR}")
    message(STATUS "--------------------------------------------")
  endif()

  string(STRIP "${${GVS_PROJECT_NAME}_BUILD_DATE}" DVERS)
  string(REPLACE  "/" "-" DVERS "${DVERS}")
  # Run 'git describe' to get our tag offset
  # execute_process(COMMAND ${GIT_EXECUTABLE} describe --long
  #                 OUTPUT_VARIABLE DVERS
  #                 RESULT_VARIABLE did_run
  #                 ERROR_VARIABLE git_error
  #                 WORKING_DIRECTORY ${${GVS_PROJECT_NAME}_SOURCE_DIR} )

  #message(STATUS "DVERS: ${DVERS}")
  set(PROJECT_PREFIX "${GVS_PROJECT_NAME}")
  set(VERSION_GEN_NAME "${GVS_PROJECT_NAME}")
  set(VERSION_GEN_NAMESPACE "${GVS_NAMESPACE}")
  string(TOLOWER "${VERSION_GEN_NAMESPACE}" VERSION_INCLUDE_GUARD)
  set(VERSION_GEN_NAMESPACE_EXPORT "${GVS_EXPORT_MACRO}")
  set(VERSION_GEN_VER_MAJOR  ${${GVS_PROJECT_NAME}_VERSION_MAJOR})
  set(VERSION_GEN_VER_MINOR  ${${GVS_PROJECT_NAME}_VERSION_MINOR})
  set(VERSION_GEN_VER_PATCH "0")
  set(VERSION_GEN_VER_REVISION "0")

  string(TIMESTAMP VERSION_BUILD_DATE "%Y/%m/%d")

  set(VERSION_GEN_HEADER_FILE_NAME ${GVS_GENERATED_HEADER_FILE_PATH})

  #-- Make sure that actually worked and if not just generate some dummy values
  if(NOT "${DVERS}" STREQUAL "")
    string(STRIP ${DVERS} DVERS)
    string(REPLACE  "-" ";" VERSION_LIST ${DVERS})
    list(LENGTH VERSION_LIST VERSION_LIST_LENGTH)

    set(VERSION_GEN_VER_PATCH "0")
    set(VERSION_GEN_VER_REVISION "0")

    list(LENGTH VERSION_LIST LIST_LENGTH)
    if(LIST_LENGTH GREATER 1)
      list(GET VERSION_LIST 0 VERSION_GEN_VER_MAJOR)
      list(GET VERSION_LIST 1 VERSION_GEN_VER_MINOR)
      list(GET VERSION_LIST 2 VERSION_GEN_VER_PATCH)
    endif()

  endif()

  find_package(Git)
  # Run 'git rev-parse --short HEAD' to get our revision
  execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
                  OUTPUT_VARIABLE DVERS
                  RESULT_VARIABLE did_run
                  ERROR_VARIABLE git_error
                  WORKING_DIRECTORY ${${GVS_PROJECT_NAME}_SOURCE_DIR} )
  string(STRIP "${DVERS}" DVERS)
  if(DVERS STREQUAL "")
    message(STATUS "[${GVS_PROJECT_NAME}] 'git rev-parse --short HEAD' did not return anything valid")
    set(VERSION_GEN_VER_REVISION "000000000000")
  else()
    set(VERSION_GEN_VER_REVISION "${DVERS}")
  endif()

  set(${GVS_PROJECT_NAME}_VERSION_MAJOR "${VERSION_GEN_VER_MAJOR}" PARENT_SCOPE)
  set(${GVS_PROJECT_NAME}_VERSION_MINOR "${VERSION_GEN_VER_MINOR}" PARENT_SCOPE)
  set(${GVS_PROJECT_NAME}_VERSION_PATCH "${VERSION_GEN_VER_PATCH}" PARENT_SCOPE)
  set(${GVS_PROJECT_NAME}_VERSION_TWEAK "${VERSION_GEN_VER_REVISION}" PARENT_SCOPE)

  set(CMP_TOP_HEADER_INCLUDE_STATMENT "")
  if(NOT "${CMP_TOP_HEADER_FILE}" STREQUAL "")
    set(CMP_TOP_HEADER_INCLUDE_STATMENT "#include \"${CMP_TOP_HEADER_FILE}\"")
  endif()

  execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --verify HEAD
                  OUTPUT_VARIABLE GVS_GIT_HASH
                  RESULT_VARIABLE did_run
                  ERROR_VARIABLE git_error
                  WORKING_DIRECTORY ${${GVS_PROJECT_NAME}_SOURCE_DIR} 
  )
  string(REPLACE "\n" "" GVS_GIT_HASH "${GVS_GIT_HASH}")
  set_property(GLOBAL PROPERTY ${GVS_PROJECT_NAME}_GIT_HASH ${GVS_GIT_HASH})

  execute_process(COMMAND ${GIT_EXECUTABLE} log -1 --pretty='%cd' --date=format:%Y-%m-%d-%H:%M:%S
                  OUTPUT_VARIABLE GVS_GIT_COMMIT_DATE
                  RESULT_VARIABLE did_run
                  ERROR_VARIABLE git_error
                  WORKING_DIRECTORY ${${GVS_PROJECT_NAME}_SOURCE_DIR} 
  )
  string(REPLACE "\n" "" GVS_GIT_COMMIT_DATE "${GVS_GIT_COMMIT_DATE}")
  set_property(GLOBAL PROPERTY ${GVS_PROJECT_NAME}_GIT_COMMIT_DATE ${GVS_GIT_COMMIT_DATE})

  if(NOT "${GVS_GENERATED_HEADER_FILE_PATH}" STREQUAL "")
    #message(STATUS "Generating: ${${GVS_PROJECT_NAME}_BINARY_DIR}/${GVS_GENERATED_HEADER_FILE_PATH}")
    cmpConfigureFileWithMD5Check( GENERATED_FILE_PATH        ${${GVS_PROJECT_NAME}_BINARY_DIR}/${GVS_GENERATED_HEADER_FILE_PATH}
                                  CONFIGURED_TEMPLATE_PATH     ${CMP_VERSION_HDR_TEMPLATE_FILE} )
  endif()
  
  if(NOT "${GVS_GENERATED_SOURCE_FILE_PATH}" STREQUAL "")
    #message(STATUS "Generating: ${${GVS_PROJECT_NAME}_BINARY_DIR}/${GVS_GENERATED_SOURCE_FILE_PATH}")
    cmpConfigureFileWithMD5Check( GENERATED_FILE_PATH        ${${GVS_PROJECT_NAME}_BINARY_DIR}/${GVS_GENERATED_SOURCE_FILE_PATH}
                                CONFIGURED_TEMPLATE_PATH     ${CMP_VERSION_SRC_TEMPLATE_FILE} )
  endif()
  
  if(NOT "${GVS_VERSION_MACRO_PATH}" STREQUAL "")
    #message(STATUS "Generating: ${${GVS_PROJECT_NAME}_BINARY_DIR}/${GVS_VERSION_MACRO_PATH}")
    cmpConfigureFileWithMD5Check( GENERATED_FILE_PATH        ${${GVS_PROJECT_NAME}_BINARY_DIR}/${GVS_VERSION_MACRO_PATH}
                                CONFIGURED_TEMPLATE_PATH     ${CMP_CONFIGURED_FILES_SOURCE_DIR}/cmpVersionMacro.h.in )
  endif()
  
endfunction()



#-------------------------------------------------------------------------------
# This macro will set all the variables necessary to have a "good" OS X Application
# bundle. The variables are as follows:
#  TARGET_NAME - which can be taken from the ${TARGET_NAME} variable is needed
#  DEBUG_EXTENSION - The extension used to denote a debug built Application. Typically
#   this is '_debug'
#  ICON_FILE_PATH - The complete path to the bundle icon file
#  VERSION_STRING - The version string that you wish to use for the bundle. For OS X
#   this string is usually XXXX.YY.ZZ in type. Look at the Apple docs for more info
#-------------------------------------------------------------------------------
macro(ConfigureMacOSXBundlePlist TARGET_NAME DEBUG_EXTENSION ICON_FILE_PATH VERSION_STRING)
  # message(STATUS "ConfigureMacOSXBundlePlist for ${PROJECT_NAME} ")
  if(CMAKE_BUILD_TYPE MATCHES "Release")
    set(DBG_EXTENSION "")
  else()
    set(DBG_EXTENSION ${DEBUG_EXTENSION})
  endif()
  get_filename_component(ICON_FILE_NAME "${ICON_FILE_PATH}" NAME)

  set_target_properties(${TARGET_NAME} PROPERTIES
    MACOSX_BUNDLE_INFO_STRING "${TARGET_NAME}${DBG_EXTENSION} Version ${VERSION_STRING}, Copyright 2018 BlueQuartz Software."
    MACOSX_BUNDLE_ICON_FILE ${ICON_FILE_NAME}
    MACOSX_BUNDLE_GUI_IDENTIFIER "${TARGET_NAME}"
    MACOSX_BUNDLE_LONG_VERSION_STRING "${TARGET_NAME}${DBG_EXTENSION} Version ${VERSION_STRING}"
    MACOSX_BUNDLE_BUNDLE_NAME ${TARGET_NAME}${DBG_EXTENSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION_STRING}
    MACOSX_BUNDLE_BUNDLE_VERSION ${VERSION_STRING}
    MACOSX_BUNDLE_COPYRIGHT "Copyright 2021, BlueQuartz Software, LLC All Rights Reserved."
    MACOSX_BUNDLE_INFO_PLIST ${H5StudioProj_SOURCE_DIR}/cmake/OSX_Tools/MacOSXBundleInfo.plist.in
  )

  set(${PROJECT_NAME}_PROJECT_SRCS ${${PROJECT_NAME}_PROJECT_SRCS} ${ICON_FILE_PATH})
  set_source_files_properties(${ICON_FILE_PATH} PROPERTIES
                              MACOSX_PACKAGE_LOCATION Resources)
endmacro()

#-------------------------------------------------------------------------------
#
#-------------------------------------------------------------------------------
macro (cmp_IDE_GENERATED_PROPERTIES SOURCE_PATH HEADERS SOURCES)
    string(REPLACE "/" "\\\\" source_group_path ${SOURCE_PATH}  )
    source_group(${source_group_path} FILES ${HEADERS} ${SOURCES})

  #-- The following is needed if we ever start to use OS X Frameworks but only
  #--  works on CMake 2.6 and greater
  #set_property(SOURCE ${HEADERS}
  #             PROPERTY MACOSX_PACKAGE_LOCATION Headers/${NAME}
  #)

endmacro (cmp_IDE_GENERATED_PROPERTIES SOURCE_PATH HEADERS SOURCES)

# --------------------------------------------------------------------
# This function should be able to correctly create an Application bundle
# based on the Qt Frameworks for any platform. There are specific
# sections to ensure that plugins and other libraries and resources are
# correctly copied into the Application Bundle. On other platforms these
# items are copied into the installation directory.
# Arguments:
#  TARGET The name of the Target to use in the add_executable() commnad
#  DEBUG_EXTENSION The file name suffix extension that Debug builds will have
#  ICON_FILE The path to the proper icon file for this platform (icns for OS X, ico for windows)
#  VERSION_MAJOR The Major version
#  VERSION_MINOR The Minor version
#  VERSION_PATCH The Patch version
#  BINARY_DIR    The binary directory where some files are created for this application
#  COMPONENT     The name of the component that is used during the packaging
#  INSTALL_DEST  The destination directory inside of the CMAKE_INSTALL_PREFIX to install everything
#
#  SOURCES   All the source files that are needed to compile the code
#  LINK_LIBRARIES Dependent libraries that are needed to properly link the executable
#  LIB_SEARCH_DIRS  A list of directories where certain dependent libraries or plugins can be found
#  QT_PLUGINS A List of Qt Plugins that this project needs
#  OTHER_PLUGINS A list of other plugins that are needed by this Application. These can be those built
#     by this project or located somewhere else.
function(Build_Qt_Application_Bundle)
    set(options )
    set(oneValueArgs TARGET DEBUG_EXTENSION ICON_FILE VERSION_MAJOR VERSION_MINOR VERSION_PATCH
                     BINARY_DIR COMPONENT INSTALL_DEST PLUGIN_LIST_FILE )
    set(multiValueArgs SOURCES LINK_LIBRARIES LIB_SEARCH_DIRS QT5_MODULES QML_MODULES QT_PLUGINS OTHER_PLUGINS)
    cmake_parse_arguments(QAB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    # Default GUI type is blank
    set(GUI_TYPE "")

    #-- Configure the OS X Bundle Plist
    if(APPLE)
        set(GUI_TYPE MACOSX_BUNDLE)
        #-- Make sure the qt_menu.nib is copied if we are using Qt Cocoa by setting the
        # source files properties of the qt_menu.nib package
        if(QT_MAC_USE_COCOA)
            get_filename_component(qt_menu_nib
              "${QT_QTGUI_LIBRARY_RELEASE}/Resources/qt_menu.nib"
              REALPATH)
            set(qt_menu_nib_sources
              "${qt_menu_nib}/classes.nib"
              "${qt_menu_nib}/info.nib"
              "${qt_menu_nib}/keyedobjects.nib"
              )
            set_source_files_properties(
              ${qt_menu_nib_sources}
              PROPERTIES
              MACOSX_PACKAGE_LOCATION Resources/qt_menu.nib
            )
        else(QT_MAC_USE_COCOA)
            set(qt_menu_nib_sources)
        endif(QT_MAC_USE_COCOA)
        list(APPEND QAB_SOURCES ${qt_menu_nib_sources})
    elseif(WIN32)
        set(GUI_TYPE WIN32)
        file (WRITE "${CMAKE_CURRENT_BINARY_DIR}/Icon.rc"
          "// Icon with lowest ID value placed first to ensure application icon\n"
          "// remains consistent on all systems.\n"
          "IDI_ICON1 ICON \"${QAB_ICON_FILE}\"")
        set(QAB_ICON_FILE "${CMAKE_CURRENT_BINARY_DIR}/Icon.rc")
        cmp_IDE_GENERATED_PROPERTIES("${TARGET}/Generated/QrcFiles" "${QAB_ICON_FILE}" "")
    endif(APPLE)

#-- Append the Icon file/Image/Resource file to the list of Sources to compile
    list(APPEND QAB_SOURCES ${QAB_ICON_FILE})

    foreach(qt5module ${QAB_QT5_MODULES})
      set(QAB_LINK_LIBRARIES ${QAB_LINK_LIBRARIES} Qt5::${qt5module})
    endforeach()

#-- Add and Link our executable
    add_executable( ${QAB_TARGET} ${GUI_TYPE} ${QAB_SOURCES} )
    target_link_libraries( ${QAB_TARGET}
                            ${QAB_LINK_LIBRARIES}
                             )

#-- Make sure we have a proper bundle icon. This must occur AFTER the add_executable command
    if(APPLE)
      ConfigureMacOSXBundlePlist( "${QAB_TARGET}" "${QAB_DEBUG_EXTENSION}" "${QAB_ICON_FILE}"
                                     "${QAB_VERSION_MAJOR}.${QAB_VERSION_MINOR}.${QAB_VERSION_PATCH}" )
    endif(APPLE)

#-- Set the Debug Suffix for the application
    set_target_properties( ${QAB_TARGET}
                PROPERTIES
                DEBUG_OUTPUT_NAME ${QAB_TARGET}${QAB_DEBUG_EXTENSION}
                RELEASE_OUTPUT_NAME ${QAB_TARGET}
    )
    # enable per object parallel compilation in this large library
    if(MSVC)
        target_compile_options(${QAB_TARGET} PRIVATE "/MP")
    endif()
    
    if(CMAKE_SYSTEM_NAME MATCHES "Linux")
      set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
      set_target_properties( ${QAB_TARGET}
              PROPERTIES
              INSTALL_RPATH \$ORIGIN/../lib)
      target_link_options(${QAB_TARGET} PUBLIC "-Wl,--disable-new-dtags")
      target_compile_options(${QAB_TARGET} PUBLIC "-no-pie")
    endif()
#-- Create install rules for any Qt Plugins that are needed
    set(pi_dest ${QAB_INSTALL_DEST}/Plugins)
    # if we are on OS X then we set the plugin installation location to inside the App bundle
    if(APPLE)
        set(pi_dest ${QAB_TARGET}.app/Contents/Plugins)
        set(osx_app_name ${QAB_TARGET})
        if(CMAKE_BUILD_TYPE MATCHES "Debug")
            set(pi_dest ${QAB_TARGET}${QAB_DEBUG_EXTENSION}.app/Contents/Plugins)
            set(osx_app_name ${QAB_TARGET}${QAB_DEBUG_EXTENSION})
        endif()
    endif()


    set(app_plugin_list "")
    set(lib_search_dirs "")
    set(write_qt_plugins_in_qtconf "")
#-- It is important as you build up the list to modify the path to the Qt Plugin
#-- to point to the plugin that will appear in the Application bundle and NOT
#-- the path to your Qt installation. If you do NOT do this step properly AND you
#-- have write privs on your Qt Installation CMake will most likely "fixup" your
#-- Qt installation files which really isn't good at all. Also when generating the
#-- list it is important to have Absolute Paths to these plugins otherwise
#-- fixup_bundle() can not find the libraries.
    foreach(pi ${QAB_QT_PLUGINS})
        set(write_qt_plugins_in_qtconf "1")
        get_filename_component(qt_plugin_name "${pi}" NAME)
        get_filename_component(qt_plugin_type_path "${pi}" PATH)
        get_filename_component(qt_plugin_type "${qt_plugin_type_path}" NAME)
        #install(PROGRAMS ${pi}
        #        DESTINATION "${pi_dest}/${qt_plugin_type}"
        #        COMPONENT ${QAB_COMPONENT}
        #)
        list(APPEND app_plugin_list "\${CMAKE_INSTALL_PREFIX}/${pi_dest}/${qt_plugin_type}/${qt_plugin_name}")
    endforeach()
    list(REMOVE_DUPLICATES lib_search_dirs)

#
#-- Create install rules for our own plugins that are targets in the build system which
#-- is only needed on Apple systems to make sure we get them installed into the bundle.
#-- On other platforms the standard installation rules are used instead.
    if(APPLE AND NOT COMPLEX_ANACONDA)
        foreach(pi ${QAB_OTHER_PLUGINS})
          get_filename_component(plugin_name "${pi}" NAME)
          install(PROGRAMS ${pi}
                  DESTINATION "${pi_dest}"
                  COMPONENT ${QAB_COMPONENT}
          )
          list(APPEND app_plugin_list "\${CMAKE_INSTALL_PREFIX}/${pi_dest}/${plugin_name}")
        endforeach()
    endif()
  
    #-- Create an Install Rule for the main app bundle target
    install(TARGETS ${QAB_TARGET}
        COMPONENT ${QAB_COMPONENT}
        RUNTIME DESTINATION ${QAB_INSTALL_DEST}
        LIBRARY DESTINATION ${QAB_INSTALL_DEST}
        ARCHIVE DESTINATION ${QAB_INSTALL_DEST}
        BUNDLE DESTINATION ${QAB_INSTALL_DEST}
    )

    get_property(COMPLEX_EXTRA_LIBRARY_DIRS GLOBAL PROPERTY COMPLEX_EXTRA_LIBRARY_DIRS)

#-- Create last install rule that will run fixup_bundle() on OS X Machines. Other platforms we
#-- are going to create the install rules elsewhere
    if(APPLE)
      set(COMPLEX_PLUGINS "")
      get_property(ComplexPluginTargets GLOBAL PROPERTY ComplexPluginTargets)
      foreach(plugTarget ${ComplexPluginTargets})
        get_target_property(output_dir ${plugTarget} FOLDER)
        get_target_property(output_suffix ${plugTarget} SUFFIX)
        set(COMPLEX_PLUGINS ${COMPLEX_PLUGINS} "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_SHARED_LIBRARY_PREFIX}${plugTarget}${output_suffix}")
      endforeach(plugTarget ${ComplexPluginTargets})
      
      list(APPEND lib_search_dirs "${QAB_LIB_SEARCH_DIRS}")

      set(OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT
                  "${QAB_BINARY_DIR}/OSX_Scripts/${QAB_TARGET}_CompleteBundle.cmake")

      configure_file("${H5StudioProj_SOURCE_DIR}/cmake/OSX_Tools/CompleteBundle.cmake.in"
              "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}" @ONLY IMMEDIATE)

      if(NOT COMPLEX_ANACONDA)
        install(SCRIPT "${OSX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}" COMPONENT ${QAB_COMPONENT})
      endif()
    endif(APPLE)

    if(WIN32)
      set(COMPLEX_PLUGINS "")
      get_property(ComplexPluginTargets GLOBAL PROPERTY ComplexPluginTargets)
      foreach(plugTarget ${ComplexPluginTargets})
        get_target_property(output_dir ${plugTarget} FOLDER)
        get_target_property(output_suffix ${plugTarget} SUFFIX)

        if(CMAKE_GENERATOR MATCHES "Visual Studio")
          set(BUILD_TYPES "Debug;Release")
          foreach(btype ${BUILD_TYPES})
              install(TARGETS ${plugTarget}
                      DESTINATION "."
                      CONFIGURATIONS ${btype}
                      COMPONENT Applications
                      ARCHIVE DESTINATION lib
                      )
          endforeach()
        else()
          set(COMPLEX_PLUGINS ${COMPLEX_PLUGINS} "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_SHARED_LIBRARY_PREFIX}${plugTarget}${output_suffix}")
        endif()
      endforeach(plugTarget ${ComplexPluginTargets})
      
      list(APPEND lib_search_dirs "${QAB_LIB_SEARCH_DIRS}")

      set(BUNDLE_UTILITIES_SCRIPT
          "${QAB_BINARY_DIR}/Windows_Scripts/${QAB_TARGET}_CompleteBundle.cmake")
      configure_file("${H5StudioProj_SOURCE_DIR}/cmake/Windows_Tools/CompleteBundle.cmake.in"
            "${BUNDLE_UTILITIES_SCRIPT}" @ONLY IMMEDIATE)
  
      install(SCRIPT "${BUNDLE_UTILITIES_SCRIPT}" COMPONENT ${QAB_COMPONENT})
    endif()

#-- This should be called when we are on Linux
    if(CMAKE_SYSTEM_NAME MATCHES "Linux")

      set(COMPLEX_PLUGINS "")
      get_property(ComplexPluginTargets GLOBAL PROPERTY ComplexPluginTargets)
      foreach(plugTarget ${ComplexPluginTargets})
        get_target_property(output_dir ${plugTarget} FOLDER)
        get_target_property(output_suffix ${plugTarget} SUFFIX)
        set(COMPLEX_PLUGINS ${COMPLEX_PLUGINS} "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${CMAKE_SHARED_LIBRARY_PREFIX}${plugTarget}${output_suffix}")
      endforeach(plugTarget ${ComplexPluginTargets})
      
      list(APPEND lib_search_dirs "${QAB_LIB_SEARCH_DIRS}")

      set(linux_app_name ${QAB_TARGET})
      set(LINUX_MAKE_STANDALONE_LAUNCH_SCRIPT
                  "${QAB_BINARY_DIR}/Linux_Scripts/${QAB_TARGET}.sh")
      set(lib_suffix "")
      set(build_type "${CMAKE_BUILD_TYPE}")
      if("${build_type}" STREQUAL "Debug")
          set(lib_suffix "_debug")
      endif()

      set(LINUX_INSTALL_LIBS_CMAKE_SCRIPT
              "${QAB_BINARY_DIR}/Linux_Scripts/${QAB_TARGET}_CompleteBundle.cmake")
      set(OPTIMIZE_BUNDLE_SHELL_SCRIPT
              "${QAB_BINARY_DIR}/Linux_Scripts/${QAB_TARGET}_InstallLibraries.sh")


      set(LINUX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT
              "${QAB_BINARY_DIR}/Linux_Scripts/${QAB_TARGET}_CompleteBundle.cmake")

      configure_file("${H5StudioProj_SOURCE_DIR}/cmake/Linux_Tools/CompleteBundle.cmake.in"
                    "${LINUX_MAKE_STANDALONE_BUNDLE_CMAKE_SCRIPT}" @ONLY IMMEDIATE)
      set(PROJECT_INSTALL_DIR ${linux_app_name})

      # configure_file("${CMP_LINUX_TOOLS_SOURCE_DIR}/InstallLibraries.sh.in"
      #               "${OPTIMIZE_BUNDLE_SHELL_SCRIPT}" @ONLY IMMEDIATE)

      if(NOT COMPLEX_ANACONDA)
        install(SCRIPT "${LINUX_INSTALL_LIBS_CMAKE_SCRIPT}" COMPONENT ${QAB_COMPONENT})
      endif()

    endif()

endfunction()
