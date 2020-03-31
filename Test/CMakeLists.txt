enable_testing()

set(PLUGIN_NAME "H5Support")
set(${PLUGIN_NAME}Test_SOURCE_DIR ${${PLUGIN_NAME}_SOURCE_DIR}/Test)
set(${PLUGIN_NAME}Test_BINARY_DIR ${${PLUGIN_NAME}_BINARY_DIR}/Test)

set(TEST_TEMP_DIR ${${PLUGIN_NAME}Test_BINARY_DIR}/Test/Temp)
# Make sure the directory is created during CMake time
file(MAKE_DIRECTORY ${TEST_TEMP_DIR})

configure_file(${${PLUGIN_NAME}_SOURCE_DIR}/Resources/CMake/TestFileLocations.h.in
  ${${PLUGIN_NAME}Test_BINARY_DIR}/${PLUGIN_NAME}TestFileLocations.h @ONLY IMMEDIATE
)

set(TEST_NAMES
  H5LiteTest
  H5UtilitiesTest
)

set(${PLUGIN_NAME}_TEST_SRCS )
set(FilterTestIncludes "")
set(TestMainFunctors "")

foreach(name ${TEST_NAMES})
  set(${PLUGIN_NAME}_TEST_SRCS
    ${${PLUGIN_NAME}_TEST_SRCS}
    "${${PLUGIN_NAME}_SOURCE_DIR}/Test/${name}.cpp"
  )
  string(CONCAT
    FilterTestIncludes
    ${FilterTestIncludes}
    "#include \"${${PLUGIN_NAME}_SOURCE_DIR}/Test/${name}.cpp\"\n"
  )

  string(CONCAT
    TestMainFunctors
   ${TestMainFunctors}
   "  ${name}()()|\n"
  )
endforeach()

string(REPLACE "|" ";" TestMainFunctors ${TestMainFunctors})

configure_file(${${PLUGIN_NAME}_SOURCE_DIR}/Resources/CMake/${PLUGIN_NAME}TestMain.cpp.in
  ${${PLUGIN_NAME}Test_BINARY_DIR}/${PLUGIN_NAME}UnitTest.cpp @ONLY
)

# Set the source files properties on each source file.
foreach(f ${${PLUGIN_NAME}_TEST_SRCS})
  set_source_files_properties( ${f} PROPERTIES HEADER_FILE_ONLY TRUE)
endforeach()

set(H5SUPPORT_GENERATED_TEST_FILES
  ${${PLUGIN_NAME}Test_BINARY_DIR}/${PLUGIN_NAME}UnitTest.cpp
  ${${PLUGIN_NAME}Test_BINARY_DIR}/${PLUGIN_NAME}TestFileLocations.h
)

add_executable(H5SupportUnitTest
  ${${PLUGIN_NAME}_TEST_SRCS}  
  ${${PLUGIN_NAME}Test_SOURCE_DIR}/UnitTestSupport.h
  ${${PLUGIN_NAME}Test_SOURCE_DIR}/H5SupportTestHelper.h
  ${H5SUPPORT_GENERATED_TEST_FILES}
)
set_target_properties(H5SupportUnitTest PROPERTIES FOLDER "H5SupportProj/Test")

source_group("Generated" FILES ${H5SUPPORT_GENERATED_TEST_FILES})
source_group("Tests" FILES ${${PLUGIN_NAME}_TEST_SRCS})

target_include_directories(H5SupportUnitTest
  PRIVATE
    ${${PLUGIN_NAME}Test_SOURCE_DIR}
    ${${PLUGIN_NAME}Test_BINARY_DIR}
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${H5Support_SOURCE_DIR}/Source>
    $<BUILD_INTERFACE:${H5Support_BINARY_DIR}>
)


set(H5Support_Link_Libs hdf5::hdf5-shared)
if(H5Support_USE_QT)
  set(QT5_RULES COPY)
  if(H5Support_INSTALL_QT5)
    list(APPEND QT5_RULES " " INSTALL)
  endif()
  find_package(Qt5 COMPONENTS Core REQUIRED)
  TargetCopyInstall(${QT5_RULES} NAME "Qt5Core" TARGET Qt5::Core)
  list(APPEND H5Support_Link_Libs Qt5::Core)
endif()

target_link_libraries(H5SupportUnitTest
  PRIVATE
    ${H5Support_Link_Libs}
)

add_test(NAME H5SupportUnitTest COMMAND H5SupportUnitTest)

if(MSVC)
  set_source_files_properties(${${PLUGIN_NAME}Test_BINARY_DIR}/${PLUGIN_NAME}UnitTest.cpp PROPERTIES COMPILE_FLAGS /bigobj)
endif()

option(H5Support_BIG_DATA_TEST "Enables test for writing more than 4 GB" OFF)

if(H5Support_BIG_DATA_TEST)
  # This is just a quick test to make sure that the latest HDF5 can actually write data
  # sets that are larger than 4GB in size
  add_executable(BigHDF5DatasetTest ${${PLUGIN_NAME}Test_SOURCE_DIR}/BigHDF5DatasetTest.cpp)
  target_link_libraries(BigHDF5DatasetTest PRIVATE H5Support::H5Support)
  set_target_properties(BigHDF5DatasetTest PROPERTIES FOLDER "H5SupportProj/Test")
  add_test(NAME BigHDF5DatasetTest COMMAND BigHDF5DatasetTest)
endif()
