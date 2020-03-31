/* ============================================================================
 * Copyright (c) 2007-2019 BlueQuartz Software, LLC
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The code contained herein was partially funded by the followig contracts:
 *    United States Air Force Prime Contract FA8650-04-C-5229
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <list>
#include <string>

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"

#include "H5SupportTestHelper.h"

#include "H5SupportTestFileLocations.h"

#include "UnitTestSupport.h"

#ifdef _MSC_VER
#define WINDOWS_LARGE_FILE_SUPPORT
#if _MSC_VER < 1400
#define snprintf _snprintf
#else
#define snprintf sprintf_s
#endif
#endif

#if defined(H5Support_NAMESPACE)
using namespace H5Support_NAMESPACE;
#endif

class H5UtilitiesTest
{
public:
  H5UtilitiesTest() = default;
  ~H5UtilitiesTest() = default;

  H5UtilitiesTest(const H5UtilitiesTest&) = delete;            // Copy Constructor Not Implemented
  H5UtilitiesTest(H5UtilitiesTest&&) = delete;                 // Move Constructor Not Implemented
  H5UtilitiesTest& operator=(const H5UtilitiesTest&) = delete; // Copy Assignment Not Implemented
  H5UtilitiesTest& operator=(H5UtilitiesTest&&) = delete;      // Move Assignment Not Implemented

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    std::remove(UnitTest::H5UtilTest::FileName.c_str());
    std::remove(UnitTest::H5UtilTest::GroupTest.c_str());
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Test()
  {
    /* Create a new file using default properties. */
    hid_t fileID = H5Fcreate(UnitTest::H5UtilTest::FileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    H5SUPPORT_REQUIRE(fileID > 0);

    // std::cout << logTime() << "----------- Testing Writing/Reading of Datasets using Raw Pointers -----------";
    H5SUPPORT_REQUIRE(H5SupportTestHelper::testWritePointer2DArrayDataset<int32_t>(fileID, &AttrSize) >= 0);

    hid_t datasetID = H5Utilities::openHDF5Object(fileID, "Pointer2DArrayDataset<H5T_NATIVE_INT32>");
    H5SUPPORT_REQUIRE(datasetID > 0);

    H5SUPPORT_REQUIRE(H5Utilities::isGroup(fileID, "/") == true);
    H5SUPPORT_REQUIRE(H5Utilities::isGroup(fileID, "Pointer2DArrayDataset<H5T_NATIVE_INT32>") == false);

    std::string objectName;
    herr_t error = H5Utilities::objectNameAtIndex(fileID, 0, objectName);
    H5SUPPORT_REQUIRE(objectName.compare("Pointer2DArrayDataset<H5T_NATIVE_INT32>") == 0);

    int32_t objectType = -1;
    error = H5Utilities::getObjectType(fileID, "Pointer2DArrayDataset<H5T_NATIVE_INT32>", objectType);
    H5SUPPORT_REQUIRE(objectType == H5O_TYPE_DATASET);
    H5SUPPORT_REQUIRE(error >= 0);

    error = H5Utilities::getObjectType(fileID, "/", objectType);
    H5SUPPORT_REQUIRE(objectType == H5O_TYPE_GROUP);
    H5SUPPORT_REQUIRE(error >= 0);

    std::string objectPath = H5Utilities::getObjectPath(datasetID, false);
    H5SUPPORT_REQUIRE(objectPath.compare("Pointer2DArrayDataset<H5T_NATIVE_INT32>") == 0);

    error = H5Utilities::closeHDF5Object(datasetID);
    H5SUPPORT_REQUIRE(error >= 0);

    H5SUPPORT_REQUIRE(H5Utilities::createGroupsFromPath("/Test Path 1", fileID) >= 0);
    H5SUPPORT_REQUIRE(H5Utilities::createGroupsFromPath("/Test Path 2/", fileID) >= 0);
    H5SUPPORT_REQUIRE(H5Utilities::createGroupsFromPath("Test Path 3/", fileID) >= 0);
    H5SUPPORT_REQUIRE(H5Utilities::createGroupsFromPath("/", fileID) < 0);
    H5SUPPORT_REQUIRE(H5Utilities::createGroupsFromPath("/Test Path 4/Test Path 7", fileID) >= 0);
    H5SUPPORT_REQUIRE(H5Utilities::createGroupsFromPath("/Test Path 5/Test Path 8/", fileID) >= 0);
    H5SUPPORT_REQUIRE(H5Utilities::createGroupsFromPath("Test Path 6/Test Path 9/", fileID) >= 0);

    hid_t groupID = H5Utilities::openHDF5Object(fileID, "Test Path 1");
    H5SUPPORT_REQUIRE(groupID > 0);
    H5SUPPORT_REQUIRE(H5Utilities::createGroupsFromPath("/Test Path 1", groupID) >= 0);
    H5SUPPORT_REQUIRE(H5Utilities::createGroupsFromPath("/Test Path 2/", groupID) >= 0);
    H5SUPPORT_REQUIRE(H5Utilities::createGroupsFromPath("Test Path 3/", groupID) >= 0);
    H5SUPPORT_REQUIRE(H5Utilities::createGroupsFromPath("/", groupID) < 0);
    H5SUPPORT_REQUIRE(H5Utilities::createGroupsFromPath("/Test Path 4/Test Path 7", groupID) >= 0);
    H5SUPPORT_REQUIRE(H5Utilities::createGroupsFromPath("/Test Path 5/Test Path 8/", groupID) >= 0);
    H5SUPPORT_REQUIRE(H5Utilities::createGroupsFromPath("Test Path 6/Test Path 9/", groupID) >= 0);
    error = H5Gclose(groupID);
    H5SUPPORT_REQUIRE(error >= 0);

    hid_t testGroupID = H5Utilities::createGroup(fileID, "test group");
    H5SUPPORT_REQUIRE(testGroupID >= 0);
    error = H5Utilities::closeHDF5Object(testGroupID);

    std::list<std::string> groups;
    error = H5Utilities::getGroupObjects(fileID, H5Utilities::CustomHDFDataTypes::Any, groups);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(groups.size() == 8);

    error = static_cast<herr_t>(H5Utilities::createGroupsForDataset("/group1/group2/group3/data", fileID));
    H5SUPPORT_REQUIRE(error >= 0);
    testGroupID = H5Utilities::openHDF5Object(fileID, "/group1/group2");
    H5SUPPORT_REQUIRE(testGroupID >= 0);
    error = H5Utilities::closeHDF5Object(testGroupID);

    bool success = H5Utilities::probeForAttribute(fileID, "Pointer2DArrayDataset<H5T_NATIVE_INT32>", "ScalarAttribute<H5T_NATIVE_INT32>");
    H5SUPPORT_REQUIRE(success == true);

    success = H5Utilities::probeForAttribute(fileID, "Pointer2DArrayDataset<H5T_NATIVE_INT32>", "ScalarAttribute<>");
    H5SUPPORT_REQUIRE(success == false);

    std::list<std::string> attributes;
    error = H5Utilities::getAllAttributeNames(fileID, "Pointer2DArrayDataset<H5T_NATIVE_INT32>", attributes);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(attributes.size() == AttrSize);

    datasetID = H5Utilities::openHDF5Object(fileID, "Pointer2DArrayDataset<H5T_NATIVE_INT32>");
    H5SUPPORT_REQUIRE(datasetID > 0);
    attributes.clear();
    error = H5Utilities::getAllAttributeNames(datasetID, attributes);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(attributes.size() == AttrSize);
    error = H5Utilities::closeHDF5Object(datasetID);
    H5SUPPORT_REQUIRE(error >= 0);

    error = H5Utilities::closeFile(fileID);
    H5SUPPORT_REQUIRE(error >= 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void StressTestCreateGroups()
  {
    std::time_t currentDateTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << std::put_time(std::localtime(&currentDateTime), "%F %T") << " Starting StressTestCreateGroups()" << '\n';
    std::array<char, 64> path;
    path.fill(0);
    herr_t error = 0;
    hid_t groupID = -1;

    /* Create a new file using default properties. */
    hid_t fileID = H5Fcreate(UnitTest::H5UtilTest::GroupTest.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    H5SUPPORT_REQUIRE(fileID > 0);

    for(int i = 0; i < 100; ++i)
    {
      currentDateTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
      std::cout << std::put_time(std::localtime(&currentDateTime), "%F %T") << " Outer Loop: " << i << '\n';
      path.fill(0);
      snprintf(path.data(), path.size(), "/%03d", i);
      groupID = H5Utilities::createGroup(fileID, path.data());
      H5SUPPORT_REQUIRE(groupID > 0);
      error = H5Gclose(groupID);
      H5SUPPORT_REQUIRE(error >= 0);

      for(int j = 0; j < 100; ++j)
      {
        snprintf(path.data(), path.size(), "/%03d/%03d", i, j);
        groupID = H5Utilities::createGroup(fileID, path.data());
        H5SUPPORT_REQUIRE(groupID > 0);
        H5SUPPORT_REQUIRE(groupID > 0);
        error = H5Gclose(groupID);
        H5SUPPORT_REQUIRE(error >= 0);

        for(int k = 0; k < 100; ++k)
        {
          snprintf(path.data(), path.size(), "/%03d/%03d/%03d", i, j, k);
          groupID = H5Utilities::createGroup(fileID, path.data());
          H5SUPPORT_REQUIRE(groupID >= 0);
          H5SUPPORT_REQUIRE(groupID > 0);
          error = H5Gclose(groupID);
          H5SUPPORT_REQUIRE(error >= 0);
        }
      }
    }
    error = H5Fclose(fileID);
    H5SUPPORT_REQUIRE(error >= 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestOpenSameFile2x()
  {
    herr_t error = -1;
    /* Create a new file using default properties. */
    hid_t fileID = H5Fcreate(UnitTest::H5UtilTest::FileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    H5SUPPORT_REQUIRE(fileID > 0);

    hid_t groupID1 = H5Utilities::createGroup(fileID, "TestGroup");
    hid_t groupID2 = H5Utilities::createGroup(fileID, "TestGroup2");

    // std::cout << logTime() << "----------- Testing Writing/Reading of Datasets using Raw Pointers -----------";
    H5SUPPORT_REQUIRE(H5SupportTestHelper::testWritePointer2DArrayDataset<int32_t>(groupID1) >= 0);
    error = H5Gclose(groupID1);
    error = H5Gclose(groupID2);
    groupID1 = 0;

    error = H5Fclose(fileID);
    fileID = 0;

    hid_t fileID2 = H5Utilities::openFile(UnitTest::H5UtilTest::FileName, true); // Open the file Read Only
    hid_t dcaGroupID1 = H5Gopen(fileID2, "TestGroup", 0);

    {
      hid_t fileId2 = H5Utilities::openFile(UnitTest::H5UtilTest::FileName, true); // Open the file Read Only
      hid_t dcaGroupID2 = H5Gopen(fileId2, "TestGroup2", 0);
      error = H5Gclose(dcaGroupID2);
      dcaGroupID2 = 0;
      error = H5Utilities::closeFile(fileId2);
      H5SUPPORT_REQUIRE(error == 0);
    }

    error = H5Gclose(dcaGroupID1);
    H5SUPPORT_REQUIRE(error == 0);

    error = H5Utilities::closeFile(fileID2);
    H5SUPPORT_REQUIRE(error == 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    H5SUPPORT_REGISTER_TEST(Test())
    H5SUPPORT_REGISTER_TEST(TestOpenSameFile2x())
    H5SUPPORT_REGISTER_TEST(RemoveTestFiles())
  }

private:
  int32_t AttrSize = 0;
};
