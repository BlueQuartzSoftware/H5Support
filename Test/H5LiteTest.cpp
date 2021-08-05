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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-04-C-5229
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <cstdio>
#include <iostream>
#include <map>
#include <string>

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"

#include "H5SupportTestHelper.h"

#include "UnitTestSupport.h"

#include "H5SupportTestFileLocations.h"

/* Methods to test
 *
 * HDFTypeFromString
 * HDFTypeForPrimitiveAsStr
 * HDFTypeForPrimitive
 * writeVectorDataset - DONE
 * writePointerDataset - DONE
 * writeScalarDataset - DONE
 * writeStringDataset - DONE
 * writeStringDataset - DONE
 * writePointerAttribute - DONE
 * writeVectorAttribute - DONE
 * writeStringAttribute - DONE
 * writeStringAttribute - DONE
 * writeStringAttributes - DONE
 * writeScalarAttribute - DONE
 * readPointerDataset - DONE
 * readVectorDataset - DONE
 * readScalarDataset - DONE
 * readStringDataset - DONE
 * readStringDataset - DONE
 * readVectorAttribute - DONE
 * readScalarAttribute - DONE
 * readPointerAttribute - DONE
 * readStringAttribute - DONE
 * readStringAttribute - DONE
 * getAttributeNDims - DONE
 * getDatasetNDims - DONE
 * getDatasetType
 * getDatasetInfo - DONE
 * getAttributeInfo - DONE
 */

#if defined(H5Support_NAMESPACE)
using namespace H5Support_NAMESPACE;
#endif

class H5LiteTest
{
public:
  H5LiteTest() = default;
  ~H5LiteTest() = default;

  H5LiteTest(const H5LiteTest&) = delete;            // Copy Constructor Not Implemented
  H5LiteTest(H5LiteTest&&) = delete;                 // Move Constructor Not Implemented
  H5LiteTest& operator=(const H5LiteTest&) = delete; // Copy Assignment Not Implemented
  H5LiteTest& operator=(H5LiteTest&&) = delete;      // Move Assignment Not Implemented

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    std::remove(UnitTest::H5LiteTest::FileName.c_str());
    std::remove(UnitTest::H5LiteTest::LargeFile.c_str());
    std::remove(UnitTest::H5LiteTest::VLengthFile.c_str());
#endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  herr_t testReadPointer1DArrayAttribute(hid_t fileID, const std::string& datasetName)
  {
    herr_t error = -1;
    herr_t returnError = error;
    std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr<T>();
    H5SUPPORT_REQUIRE(attributeKey.empty() == false);
    attributeKey = "1DArrayAttribute<" + attributeKey + ">";
    std::vector<T> referenceData(H5SupportTestHelper::Constants::dim0, 0);
    for(size_t i = 0; i < H5SupportTestHelper::Constants::dim0; ++i)
    {
      referenceData[i] = static_cast<T>(i);
    }

    H5T_class_t attributeType;
    size_t attributeSize;
    hid_t typeID = -1;
    std::vector<hsize_t> dims; // Reusable for the loop
    error = H5Lite::getAttributeInfo(fileID, datasetName, attributeKey, dims, attributeType, attributeSize, typeID);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(dims.size() == 1);
    H5SUPPORT_REQUIRE(attributeSize == sizeof(T));
    hid_t rank = 0;
    error = H5Lite::getAttributeNDims(fileID, datasetName, attributeKey, rank);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(rank == 1);
    CloseH5T(typeID, error, returnError); // Close the H5A type Id that was retrieved during the loop

    size_t numElements = std::accumulate(dims.cbegin(), dims.cend(), static_cast<hsize_t>(1), std::multiplies<hsize_t>());

    std::vector<T> data(numElements, 0);
    error = H5Lite::readPointerAttribute<T>(fileID, datasetName, attributeKey, data.data());
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(data == referenceData);

    return returnError;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  herr_t testReadPointer2DArrayAttribute(hid_t fileID, const std::string& datasetName)
  {
    herr_t error = -1;
    herr_t returnError = error;
    std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr<T>();
    H5SUPPORT_REQUIRE(attributeKey.empty() == false);
    attributeKey = "2DArrayAttribute<" + attributeKey + ">";

    constexpr size_t dataSize = H5SupportTestHelper::Constants::dim0 * H5SupportTestHelper::Constants::dim1;
    std::vector<T> referenceData(dataSize, 0);

    for(size_t i = 0; i < H5SupportTestHelper::Constants::dim0; ++i)
    {
      for(size_t j = 0; j < H5SupportTestHelper::Constants::dim1; ++j)
      {
        size_t index = (H5SupportTestHelper::Constants::dim1 * i) + j;
        referenceData[index] = static_cast<T>(i * j);
      }
    }
    H5T_class_t attributeType;
    size_t attributeSize;
    hid_t typeID = -1;
    std::vector<hsize_t> dims; // Reusable for the loop
    error = H5Lite::getAttributeInfo(fileID, datasetName, attributeKey, dims, attributeType, attributeSize, typeID);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(dims.size() == 2);
    H5SUPPORT_REQUIRE(attributeSize == sizeof(T));
    hid_t rank = 0;
    error = H5Lite::getAttributeNDims(fileID, datasetName, attributeKey, rank);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(rank == 2);

    CloseH5T(typeID, error, returnError); // Close the H5A type Id that was retrieved during the loop
    size_t numElements = std::accumulate(dims.cbegin(), dims.cend(), static_cast<hsize_t>(1), std::multiplies<hsize_t>());
    std::vector<T> data(numElements, 0);
    error = H5Lite::readPointerAttribute<T>(fileID, datasetName, attributeKey, data.data());
    H5SUPPORT_REQUIRE(error >= 0);

    H5SUPPORT_REQUIRE(data == referenceData);
    return returnError;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  herr_t testReadPointer3DArrayAttribute(hid_t fileID, const std::string& datasetName)
  {
    herr_t error = -1;
    herr_t returnError = error;
    std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr<T>();
    H5SUPPORT_REQUIRE(attributeKey.empty() == false);
    attributeKey = "3DArrayAttribute<" + attributeKey + ">";

    constexpr size_t dataSize = H5SupportTestHelper::Constants::dim0 * H5SupportTestHelper::Constants::dim1 * H5SupportTestHelper::Constants::dim2;
    std::vector<T> referenceData(dataSize, 0);

    for(size_t i = 0; i < H5SupportTestHelper::Constants::dim0; ++i)
    {
      for(size_t j = 0; j < H5SupportTestHelper::Constants::dim1; ++j)
      {
        for(size_t k = 0; k < H5SupportTestHelper::Constants::dim2; ++k)
        {
          size_t index = ((H5SupportTestHelper::Constants::dim1 * i + j)) * H5SupportTestHelper::Constants::dim2 + k;
          referenceData[index] = static_cast<T>(i * j * k);
        }
      }
    }
    H5T_class_t attributeType;
    size_t attributeSize;
    hid_t typeID = -1;
    std::vector<hsize_t> dims; // Reusable for the loop
    error = H5Lite::getAttributeInfo(fileID, datasetName, attributeKey, dims, attributeType, attributeSize, typeID);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(dims.size() == 3);
    H5SUPPORT_REQUIRE(attributeSize == sizeof(T));
    hid_t rank = 0;
    error = H5Lite::getAttributeNDims(fileID, datasetName, attributeKey, rank);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(rank == 3);
    CloseH5T(typeID, error, returnError); // Close the H5A type Id that was retrieved during the loop
    size_t numElements = std::accumulate(dims.cbegin(), dims.cend(), static_cast<hsize_t>(1), std::multiplies<hsize_t>());
    std::vector<T> data(numElements, 0);
    error = H5Lite::readPointerAttribute<T>(fileID, datasetName, attributeKey, data.data());
    H5SUPPORT_REQUIRE(error >= 0);

    H5SUPPORT_REQUIRE(data == referenceData);
    return returnError;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  herr_t testReadVectorAttribute(hid_t fileID, std::string datasetName)
  {
    herr_t error = -1;
    std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr<T>();
    H5SUPPORT_REQUIRE(attributeKey.empty() == false);
    attributeKey = "VectorAttribute<" + attributeKey + ">";

    int32_t numElements = H5SupportTestHelper::Constants::dim0;
    std::vector<hsize_t> dims{H5SupportTestHelper::Constants::dim0};

    std::vector<T> data(H5SupportTestHelper::Constants::dim0, 0);
    for(int32_t i = 0; i < numElements; ++i)
    {
      data[i] = static_cast<T>(i);
    }
    std::vector<T> rData(numElements, 0); // allocate and zero out the memory
    error = H5Lite::readVectorAttribute(fileID, datasetName, attributeKey, rData);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(data == rData);
    return error;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  herr_t testReadScalarAttribute(hid_t fileID, const std::string& datasetName)
  {
    T value = 0x0F;
    T refValue = value;
    herr_t error = -1;
    std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr<T>();
    H5SUPPORT_REQUIRE(attributeKey.empty() == false);
    attributeKey = "ScalarAttribute<" + attributeKey + ">";

    error = H5Lite::readScalarAttribute(fileID, datasetName, attributeKey, value);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(refValue == value);
    return error;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  herr_t testWriteVectorDataset(hid_t fileID)
  {
    herr_t error = 1;

    std::vector<hsize_t> dims(H5SupportTestHelper::Constants::rank1D, H5SupportTestHelper::Constants::dim0);
    /* Make dataset char */
    std::vector<T> data(H5SupportTestHelper::Constants::dim0, 0);
    for(int32_t i = 0; i < H5SupportTestHelper::Constants::dim0; ++i)
    {
      data[i] = static_cast<T>(i) * static_cast<T>(5);
    }

    std::string datasetName = H5Lite::HDFTypeForPrimitiveAsStr<T>();
    datasetName = "VectorDataset<" + datasetName + ">";
    std::cout << "Running " << datasetName << " ... " << '\n';
    error = H5Lite::writeVectorDataset<T>(fileID, datasetName, dims, data);
    H5SUPPORT_REQUIRE(error >= 0);

    std::cout << " Passed" << '\n';

    return error;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  herr_t testReadVectorDataset(hid_t fileID)
  {
    herr_t error = 1;

    std::vector<hsize_t> dims(H5SupportTestHelper::Constants::rank1D, H5SupportTestHelper::Constants::dim0);
    std::vector<T> referenceData(H5SupportTestHelper::Constants::dim0, 0);
    for(int32_t i = 0; i < H5SupportTestHelper::Constants::dim0; ++i)
    {
      referenceData[i] = static_cast<T>(i) * static_cast<T>(5);
    }

    std::string datasetName = H5Lite::HDFTypeForPrimitiveAsStr<T>();
    std::cout << "Running testReadVectorDataset <" + datasetName + "> ... " << '\n';
    datasetName = "VectorDataset<" + datasetName + ">";

    std::vector<T> data;
    error = H5Lite::readVectorDataset(fileID, datasetName, data);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(data == referenceData);

    std::cout << " Passed" << '\n';

    return error;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  herr_t testWriteScalarDataset(hid_t fileID)
  {
    T value = 0x0F;
    herr_t error = 1;

    std::string datasetName = H5Lite::HDFTypeForPrimitiveAsStr<T>();
    datasetName = "ScalarDataset<" + datasetName + ">";
    std::cout << "Running " << datasetName << " ... " << '\n';
    error = H5Lite::writeScalarDataset(fileID, datasetName, value);
    H5SUPPORT_REQUIRE(error >= 0);

    bool exists = H5Lite::datasetExists(fileID, datasetName);
    H5SUPPORT_REQUIRE_EQUAL(exists, true);

    exists = H5Lite::datasetExists(fileID, std::string("DOES_NOT_EXIST"));
    H5SUPPORT_REQUIRE_EQUAL(exists, false);

    std::cout << " Passed" << '\n';

    return error;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  template <typename T>
  herr_t testReadScalarDataset(hid_t fileID)
  {
    T value = 0x0F;
    T refValue = value;
    herr_t error = 1;

    std::string datasetName = H5Lite::HDFTypeForPrimitiveAsStr<T>();
    std::cout << "Running testReadScalarDataset <" + datasetName + "> ... " << '\n';
    datasetName = "ScalarDataset<" + datasetName + ">";

    error = H5Lite::readScalarDataset(fileID, datasetName, value);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(refValue == value);

    std::cout << " Passed" << '\n';

    return error;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  herr_t testWriteStringDatasetAndAttributes(hid_t fileID)
  {
    std::cout << "Running testWriteStringDatasetAndAttributes ......" << '\n';
    herr_t error = 0;
    std::string datasetName("std::string");
    std::string datasetNameBytes("c_string");
    std::string attributeKey("std::string");

    std::string strData("THIS IS THE DATA");
    std::string strDataBytes("THIS IS THE DATA AS BYTES");
    std::string attrData("THIS IS THE ATTRIBUTE DATA");
    std::string attrDataBytes("THIS IS THE ATTRIBUTE DATA AS BYTES");

    // Write a String Data set using a std::string as the data source
    error = H5Lite::writeStringDataset(fileID, datasetName, strData);
    H5SUPPORT_REQUIRE(error >= 0);

    // Over Write the data from Above but with different values
    strData = "THIS IS XXX DATA";
    error = H5Lite::writeStringDataset(fileID, datasetName, strData);
    H5SUPPORT_REQUIRE(error >= 0);

    // Over Write the data from Above but with longer string
    strData = "THIS IS LONGER DATA";
    error = H5Lite::writeStringDataset(fileID, datasetName, strData);
    H5SUPPORT_REQUIRE(error >= 0);

    // Over Write the data from Above but with shorter string
    strData = "THIS IS LESS DATA";
    error = H5Lite::writeStringDataset(fileID, datasetName, strData);
    H5SUPPORT_REQUIRE(error >= 0);

    // Over Write the data from Above but with even shorter string
    strData = "Even LESS DATA";
    error = H5Lite::writeStringDataset(fileID, datasetName, strData);
    H5SUPPORT_REQUIRE(error >= 0);

    // Over Write the data from Above but with even shorter string
    strData = "THIS IS THE DATA";
    error = H5Lite::writeStringDataset(fileID, datasetName, strData);
    H5SUPPORT_REQUIRE(error >= 0);

    // Write a String attribute using a std::string as the data source
    error = H5Lite::writeStringAttribute(fileID, datasetName, attributeKey, attrData);
    H5SUPPORT_REQUIRE(error >= 0);

    attributeKey = "c_string";
    error = H5Lite::writeStringAttribute(fileID, datasetName, attributeKey, attrDataBytes.size() + 1, attrDataBytes.data());

    // Write a String data set using char* as data source
    error = H5Lite::writeStringDataset(fileID, datasetNameBytes, strDataBytes.size() + 1, strDataBytes.data());
    H5SUPPORT_REQUIRE(error >= 0);

    // Write a bunch of Attributes using a Map structure.
    std::map<std::string, std::string> attrMap;
    attrMap[attributeKey] = attrData;
    attributeKey = "std::string";
    attrMap[attributeKey] = attrData;

    error = H5Lite::writeStringAttributes(fileID, datasetNameBytes, attrMap);
    H5SUPPORT_REQUIRE(error >= 0);

    std::cout << " Passed" << '\n';
    return error;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  herr_t HDF5Dump_datatype(hid_t dataset_id, std::ostream& iostr)
  {
    herr_t status = 0;

    hid_t type_id = H5Dget_type(dataset_id);

    iostr << "    ";
    iostr << "DATATYPE \"";

    switch(H5Tget_class(type_id))
    {
    case H5T_INTEGER:
      iostr << "Integer";
      break;

    case H5T_FLOAT:
      if(H5Tequal(type_id, H5T_IEEE_F32BE) || H5Tequal(type_id, H5T_IEEE_F32LE) || H5Tequal(type_id, H5T_NATIVE_FLOAT))
      {
        // Float
        iostr << "Float";
      }
      else if(H5Tequal(type_id, H5T_IEEE_F64BE) || H5Tequal(type_id, H5T_IEEE_F64LE) || H5Tequal(type_id, H5T_NATIVE_DOUBLE) || H5Tequal(type_id, H5T_NATIVE_LDOUBLE))
      {
        // Double
        iostr << "Double";
      }
      else
      {
        iostr << "Undefined HDF5 float.";
      }
      break;

    case H5T_STRING:
      iostr << "String - Unsupported";
      break;

    case H5T_COMPOUND:
      iostr << "Compound - Unsupported";
      break;

    default:
      iostr << "Unsupported or unknown data type";
      break;
    }

    iostr << "\""
          << "\n";

    status = H5Tclose(type_id);

    return status;
  }

  // -----------------------------------------------------------------------------
  //  Uses Raw Pointers to read data from the data file
  // -----------------------------------------------------------------------------
  template <typename T>
  herr_t testReadPointer2DArrayDataset(hid_t fileID)
  {
    // Create the reference data
    herr_t error = 1;
    std::vector<hsize_t> dims{H5SupportTestHelper::Constants::dim0, H5SupportTestHelper::Constants::dim1};

    size_t tSize = std::accumulate(dims.cbegin(), dims.cend(), static_cast<hsize_t>(1), std::multiplies<hsize_t>());
    std::vector<T> referenceData(tSize);
    for(int32_t i = 0; i < tSize; ++i)
    {
      referenceData[i] = static_cast<T>(i) * static_cast<T>(5);
    }
    // Generate the data set name
    std::string datasetName = H5Lite::HDFTypeForPrimitiveAsStr<T>();
    std::cout << "Running testReadPointer2DArrayDataset<" << datasetName << "> ... " << '\n';
    datasetName = "Pointer2DArrayDataset<" + datasetName + ">";

    H5T_class_t attributeType;
    size_t attributeSize;

    error = H5Lite::getDatasetInfo(fileID, "FOOBAR", dims, attributeType, attributeSize);
    H5SUPPORT_REQUIRE(error < 0);

    error = H5Lite::getDatasetInfo(fileID, datasetName, dims, attributeType, attributeSize);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(dims.size() == 2);
    H5SUPPORT_REQUIRE(attributeSize == sizeof(T));
    hid_t rank = 0;
    error = H5Lite::getDatasetNDims(fileID, datasetName, rank);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(rank == 2);

    size_t numElements = std::accumulate(dims.cbegin(), dims.cend(), static_cast<hsize_t>(1), std::multiplies<hsize_t>());

    herr_t returnError = 0;
    hid_t datasetID = -1;
    /* Open the dataset. */
    if((datasetID = H5Dopen(fileID, datasetName.data(), H5P_DEFAULT)) < 0)
    {
      return -2;
    }
    /* Get an identifier for the datatype. */
    hid_t typeID = H5Dget_type(datasetID);

    error = H5Dclose(datasetID);
    if(error < 0)
    {
      std::cout << "File: " << __FILE__ << "(" << __LINE__ << "): "
                << "Error Closing Dataset."
                << "\n";
      returnError = error;
    }
    if(returnError < 0)
    {
      return returnError;
    }

    H5SUPPORT_REQUIRE(typeID > 0);
    //  std::cout << H5Lite::StringForHDFType(typeID) << "\n";
    error = H5Tclose(typeID);
    H5SUPPORT_REQUIRE(error >= 0);

    hid_t dsType = H5Lite::getDatasetType(fileID, datasetName);
    H5SUPPORT_REQUIRE(dsType > 0);
    //  std::cout << H5Lite::StringForHDFType(dsType) << "\n";

    error = H5Tclose(dsType);
    H5SUPPORT_REQUIRE(error >= 0);

    std::vector<T> data(numElements, 0);
    error = H5Lite::readPointerDataset(fileID, datasetName, data.data());
    H5SUPPORT_REQUIRE(error >= 0);
    // Compare the data...
    H5SUPPORT_REQUIRE(data == referenceData);

    // Read all the attributes
    error = testReadPointer1DArrayAttribute<int8_t>(fileID, datasetName);
    error = testReadPointer1DArrayAttribute<uint8_t>(fileID, datasetName);
    error = testReadPointer1DArrayAttribute<int16_t>(fileID, datasetName);
    error = testReadPointer1DArrayAttribute<uint16_t>(fileID, datasetName);
    error = testReadPointer1DArrayAttribute<int32_t>(fileID, datasetName);
    error = testReadPointer1DArrayAttribute<uint32_t>(fileID, datasetName);
    error = testReadPointer1DArrayAttribute<int64_t>(fileID, datasetName);
    error = testReadPointer1DArrayAttribute<uint64_t>(fileID, datasetName);
    error = testReadPointer1DArrayAttribute<float>(fileID, datasetName);
    error = testReadPointer1DArrayAttribute<double>(fileID, datasetName);

    error = testReadPointer2DArrayAttribute<int8_t>(fileID, datasetName);
    error = testReadPointer2DArrayAttribute<uint8_t>(fileID, datasetName);
    error = testReadPointer2DArrayAttribute<int16_t>(fileID, datasetName);
    error = testReadPointer2DArrayAttribute<uint16_t>(fileID, datasetName);
    error = testReadPointer2DArrayAttribute<int32_t>(fileID, datasetName);
    error = testReadPointer2DArrayAttribute<uint32_t>(fileID, datasetName);
    error = testReadPointer2DArrayAttribute<int64_t>(fileID, datasetName);
    error = testReadPointer2DArrayAttribute<uint64_t>(fileID, datasetName);
    error = testReadPointer2DArrayAttribute<float>(fileID, datasetName);
    error = testReadPointer2DArrayAttribute<double>(fileID, datasetName);

    error = testReadPointer3DArrayAttribute<int8_t>(fileID, datasetName);
    error = testReadPointer3DArrayAttribute<uint8_t>(fileID, datasetName);
    error = testReadPointer3DArrayAttribute<int16_t>(fileID, datasetName);
    error = testReadPointer3DArrayAttribute<uint16_t>(fileID, datasetName);
    error = testReadPointer3DArrayAttribute<int32_t>(fileID, datasetName);
    error = testReadPointer3DArrayAttribute<uint32_t>(fileID, datasetName);
    error = testReadPointer3DArrayAttribute<int64_t>(fileID, datasetName);
    error = testReadPointer3DArrayAttribute<uint64_t>(fileID, datasetName);
    error = testReadPointer3DArrayAttribute<float>(fileID, datasetName);
    error = testReadPointer3DArrayAttribute<double>(fileID, datasetName);
    //
    error = testReadVectorAttribute<int8_t>(fileID, datasetName);
    error = testReadVectorAttribute<uint8_t>(fileID, datasetName);
    error = testReadVectorAttribute<int16_t>(fileID, datasetName);
    error = testReadVectorAttribute<uint16_t>(fileID, datasetName);
    error = testReadVectorAttribute<int32_t>(fileID, datasetName);
    error = testReadVectorAttribute<uint32_t>(fileID, datasetName);
    error = testReadVectorAttribute<int64_t>(fileID, datasetName);
    error = testReadVectorAttribute<uint64_t>(fileID, datasetName);
    error = testReadVectorAttribute<float>(fileID, datasetName);
    error = testReadVectorAttribute<double>(fileID, datasetName);

    error = testReadScalarAttribute<int8_t>(fileID, datasetName);
    error = testReadScalarAttribute<uint8_t>(fileID, datasetName);
    error = testReadScalarAttribute<int16_t>(fileID, datasetName);
    error = testReadScalarAttribute<uint16_t>(fileID, datasetName);
    error = testReadScalarAttribute<int32_t>(fileID, datasetName);
    error = testReadScalarAttribute<uint32_t>(fileID, datasetName);
    error = testReadScalarAttribute<int64_t>(fileID, datasetName);
    error = testReadScalarAttribute<uint64_t>(fileID, datasetName);
    error = testReadScalarAttribute<float>(fileID, datasetName);
    error = testReadScalarAttribute<double>(fileID, datasetName);

    std::cout << " Passed"
              << "\n";
    return error;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  herr_t testReadStringDatasetAndAttributes(hid_t fileID)
  {
    std::cout << "Running testReadStringDatasetAndAttributes ......" << '\n';
    herr_t error = 0;

    std::string datasetName("std::string");
    std::string datasetNameBytes("c_string");
    std::string attributeKey("std::string");

    std::string refData("THIS IS THE DATA");
    std::string refDataBytesStr("THIS IS THE DATA AS BYTES");
    std::vector<char> refDataBytes;
    std::copy(refDataBytesStr.c_str(), refDataBytesStr.c_str() + refDataBytesStr.size() + 1, std::back_inserter(refDataBytes));

    std::string refAttrData("THIS IS THE ATTRIBUTE DATA");
    std::string refAttrDataBytesStr("THIS IS THE ATTRIBUTE DATA AS BYTES");
    std::vector<char> refAttrDataBytes;
    std::copy(refAttrDataBytesStr.c_str(), refAttrDataBytesStr.c_str() + refAttrDataBytesStr.size() + 1, std::back_inserter(refAttrDataBytes));

    std::string strData("");
    // Read the string as a std::string from the file
    error = H5Lite::readStringDataset(fileID, datasetName, strData);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(refData.compare(strData) == 0);

    // Read the string again this time passing in a std::string that has data in it which should get cleared
    strData = "FooBarJunk";
    error = H5Lite::readStringDataset(fileID, datasetName, strData);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(refData.compare(strData) == 0);

    // Read the Attributes
    std::string attrData("");
    // Read the Attribute as a std::string and compare to the reference
    error = H5Lite::readStringAttribute(fileID, datasetName, attributeKey, attrData);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(refAttrData.compare(attrData) == 0);

    // Read the attribute as a byte array (US ASCII nullptr TERM)
    std::vector<char> attrDataPtr(refAttrDataBytes.size(), 0); // Create a null terminated string initialized to all zeros
    attributeKey = "c_string";
    error = H5Lite::readStringAttribute(fileID, datasetName, attributeKey, attrDataPtr.data());
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(attrDataPtr == refAttrDataBytes);

    // Read a string data set which was written as a std::vector<char> into a std::vector<char>
    std::vector<char> strDataBytes(refDataBytes.size(), 0);
    error = H5Lite::readStringDataset(fileID, datasetNameBytes, strDataBytes.data());
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(strDataBytes == refDataBytes);

    std::string mapString;
    error = H5Lite::readStringAttribute(fileID, datasetNameBytes, datasetNameBytes, mapString);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(refAttrData.compare(mapString) == 0);

    error = H5Lite::readStringAttribute(fileID, datasetNameBytes, attributeKey, mapString);
    H5SUPPORT_REQUIRE(error >= 0);
    H5SUPPORT_REQUIRE(refAttrData.compare(mapString) == 0);

    std::cout << " Passed" << '\n';

    return error;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void Test()
  {
    /* Create a new file using default properties. */
    hid_t fileID = H5Fcreate(UnitTest::H5LiteTest::FileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    H5SUPPORT_REQUIRE(fileID > 0);
    // Create the Extra Groups
    hid_t sintGid = H5Gcreate(fileID, "Signed Int", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    hid_t uintGid = H5Gcreate(fileID, "Unsigned Int", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    hid_t pointerGid = H5Gcreate(fileID, "Pointer", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    herr_t error = H5Gclose(sintGid);
    if(error < 0)
    {
      std::cout << "Error closing Group sintGid" << '\n';
      return;
    }
    error = H5Gclose(uintGid);
    if(error < 0)
    {
      std::cout << "Error closing Group uintGid" << '\n';
      return;
    }
    error = H5Gclose(pointerGid);
    if(error < 0)
    {
      std::cout << "Error closing Group pointerGid" << '\n';
      return;
    }

    // std::cout << logTime() << "----------- Testing Writing/Reading of Datasets using Raw Pointers -----------" << "\n";
    H5SUPPORT_REQUIRE(H5SupportTestHelper::testWritePointer2DArrayDataset<int8_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(H5SupportTestHelper::testWritePointer2DArrayDataset<uint8_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(H5SupportTestHelper::testWritePointer2DArrayDataset<int16_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(H5SupportTestHelper::testWritePointer2DArrayDataset<uint16_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(H5SupportTestHelper::testWritePointer2DArrayDataset<int32_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(H5SupportTestHelper::testWritePointer2DArrayDataset<uint32_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(H5SupportTestHelper::testWritePointer2DArrayDataset<int64_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(H5SupportTestHelper::testWritePointer2DArrayDataset<uint64_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(H5SupportTestHelper::testWritePointer2DArrayDataset<float>(fileID) >= 0);
    H5SUPPORT_REQUIRE(H5SupportTestHelper::testWritePointer2DArrayDataset<double>(fileID) >= 0);

    H5SUPPORT_REQUIRE(testWriteVectorDataset<int8_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testWriteVectorDataset<uint8_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testWriteVectorDataset<int16_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testWriteVectorDataset<uint16_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testWriteVectorDataset<int32_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testWriteVectorDataset<uint32_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testWriteVectorDataset<int64_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testWriteVectorDataset<uint64_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testWriteVectorDataset<float>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testWriteVectorDataset<double>(fileID) >= 0);

    H5SUPPORT_REQUIRE(testWriteScalarDataset<int8_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testWriteScalarDataset<uint8_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testWriteScalarDataset<int16_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testWriteScalarDataset<uint16_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testWriteScalarDataset<int32_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testWriteScalarDataset<uint32_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testWriteScalarDataset<int64_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testWriteScalarDataset<uint64_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testWriteScalarDataset<float>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testWriteScalarDataset<double>(fileID) >= 0);

    H5SUPPORT_REQUIRE(testWriteStringDatasetAndAttributes(fileID) >= 0);

    //  // ******************* Test Reading Data *************************************
    H5SUPPORT_REQUIRE(testReadPointer2DArrayDataset<int8_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadPointer2DArrayDataset<uint8_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadPointer2DArrayDataset<int16_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadPointer2DArrayDataset<uint16_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadPointer2DArrayDataset<int32_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadPointer2DArrayDataset<uint32_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadPointer2DArrayDataset<int64_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadPointer2DArrayDataset<uint64_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadPointer2DArrayDataset<float>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadPointer2DArrayDataset<double>(fileID) >= 0);

    H5SUPPORT_REQUIRE(testReadVectorDataset<int8_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadVectorDataset<uint8_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadVectorDataset<int16_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadVectorDataset<uint16_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadVectorDataset<int32_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadVectorDataset<uint32_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadVectorDataset<int64_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadVectorDataset<uint64_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadVectorDataset<float>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadVectorDataset<double>(fileID) >= 0);
    //
    H5SUPPORT_REQUIRE(testReadScalarDataset<int8_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadScalarDataset<uint8_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadScalarDataset<int16_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadScalarDataset<uint16_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadScalarDataset<int32_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadScalarDataset<uint32_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadScalarDataset<int64_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadScalarDataset<uint64_t>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadScalarDataset<float>(fileID) >= 0);
    H5SUPPORT_REQUIRE(testReadScalarDataset<double>(fileID) >= 0);

    H5SUPPORT_REQUIRE(testReadStringDatasetAndAttributes(fileID) >= 0);

    /* Close the file. */
    H5Fclose(fileID);
    // std::cout << logTime() << "Testing Complete" << "\n";
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestLargeFileSupport()
  {
    /* Create a new file using default properties. */
    hid_t fileID = H5Fcreate(UnitTest::H5LiteTest::LargeFile.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    H5SUPPORT_REQUIRE(fileID > 0);
    std::vector<int> buffer(1000000); // Create a 4 MegaByte Buffer
    int32_t rank = 1;
    std::vector<hsize_t> dims(1, 1000000);
    std::string datasetName;
    for(int i = 0; i < 1000; ++i)
    {
      datasetName = "/" + std::to_string(i);
      H5Lite::writePointerDataset<int>(fileID, datasetName, rank, dims.data(), buffer.data());
      std::cout << "Large File " << i << "/1000" << '\n';
    }

    herr_t error = H5Fclose(fileID);
    H5SUPPORT_REQUIRE(error >= 0);
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestTypeDetection()
  {
    H5SUPPORT_REQUIRE_EQUAL(H5Lite::HDFTypeForPrimitive<int8_t>(), H5T_NATIVE_INT8)
    if constexpr(std::is_signed_v<char>)
    {
      H5SUPPORT_REQUIRE_EQUAL(H5Lite::HDFTypeForPrimitive<char>(), H5T_NATIVE_INT8)
    }
    else
    {
      H5SUPPORT_REQUIRE_EQUAL(H5Lite::HDFTypeForPrimitive<char>(), H5T_NATIVE_UINT8)
    }
    H5SUPPORT_REQUIRE_EQUAL(H5Lite::HDFTypeForPrimitive<signed char>(), H5T_NATIVE_INT8)
    H5SUPPORT_REQUIRE_EQUAL(H5Lite::HDFTypeForPrimitive<unsigned char>(), H5T_NATIVE_UINT8)
    H5SUPPORT_REQUIRE_EQUAL(H5Lite::HDFTypeForPrimitive<uint8_t>(), H5T_NATIVE_UINT8)
  }

  class WriteString
  {
  public:
    WriteString(hid_t dataset, hid_t datatype, hid_t dataspace, hid_t memspace)
    : m_dataset(dataset)
    , m_datatype(datatype)
    , m_dataspace(dataspace)
    , m_memspace(memspace)
    , m_pos(0)
    {
    }

  private:
    hid_t m_dataset;
    hid_t m_datatype;
    hid_t m_dataspace;
    hid_t m_memspace;
    int m_pos;

  public:
    void operator()(std::vector<std::string>::value_type const& v)
    {
      // Select the file position, 1 record at position 'pos'
      hsize_t count[] = {1};
      hsize_t offset[] = {static_cast<hsize_t>(m_pos)};
      m_pos++;
      H5Sselect_hyperslab(m_dataspace, H5S_SELECT_SET, offset, nullptr, count, nullptr);

      const char* s = v.c_str();
      H5Dwrite(m_dataset, m_datatype, m_memspace, m_dataspace, H5P_DEFAULT, &s);
    }
  };

  // -----------------------------------------------------------------------------
  //  Use unit test framework
  // -----------------------------------------------------------------------------
  void TestVLengStringReadWrite()
  {
    // Write a Single String as a VLength String and read back as a normal string.
    {
      hid_t fileID = H5Utilities::createFile(UnitTest::H5LiteTest::VLengthFile);

      std::vector<std::string> strings;
      strings.push_back("Testing 1 2 3");
      herr_t error = H5Lite::writeVectorOfStringsDataset(fileID, "VlenStrings", strings);
      H5SUPPORT_REQUIRE(error >= 0)
      H5Utilities::closeFile(fileID);

      fileID = H5Utilities::openFile(UnitTest::H5LiteTest::VLengthFile, true);

      std::string data;
      error = H5Lite::readStringDataset(fileID, "VlenStrings", data);
      H5SUPPORT_REQUIRE(error > -1)
      H5SUPPORT_REQUIRE(data.size() == 13)

      H5Utilities::closeFile(fileID);
    }

    {
      hid_t fileID = H5Utilities::createFile(UnitTest::H5LiteTest::VLengthFile);

      std::vector<std::string> strings;
      strings.push_back("Testing 1 2 3");
      strings.push_back("String with a\n   newline");
      strings.push_back("Some other String");
      strings.push_back("Titanium");
      strings.push_back("Nickel");

      herr_t error = H5Lite::writeVectorOfStringsDataset(fileID, "VlenStrings", strings);
      H5SUPPORT_REQUIRE(error >= 0)

      H5Utilities::closeFile(fileID);
    }

    {
      hid_t fileID = H5Utilities::openFile(UnitTest::H5LiteTest::VLengthFile, true);

      std::vector<std::string> data;
      H5Lite::readVectorOfStringDataset(fileID, "VlenStrings", data);
      H5SUPPORT_REQUIRE(data.size() == 5)

      H5Utilities::closeFile(fileID);
    }

    {
      hid_t fileID = H5Utilities::createFile(UnitTest::H5LiteTest::VLengthFile);

      std::vector<std::string> strings;
      strings.push_back("Testing 1 2 3");
      strings.push_back("String with a\n   newline");
      strings.push_back("Some other String");
      strings.push_back("Titanium");
      strings.push_back("Nickel");

      herr_t error = H5Lite::writeVectorOfStringsDataset(fileID, "VlenStrings", strings);
      H5SUPPORT_REQUIRE(error >= 0)

      H5Utilities::closeFile(fileID);
    }

    {
      hid_t fileID = H5Utilities::openFile(UnitTest::H5LiteTest::VLengthFile, true);

      std::vector<std::string> data;
      H5Lite::readVectorOfStringDataset(fileID, "VlenStrings", data);
      H5SUPPORT_REQUIRE(data.size() == 5)

      H5Utilities::closeFile(fileID);
    }
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    H5SUPPORT_REGISTER_TEST(TestVLengStringReadWrite())
    H5SUPPORT_REGISTER_TEST(TestTypeDetection())
    H5SUPPORT_REGISTER_TEST(Test())
    H5SUPPORT_REGISTER_TEST(RemoveTestFiles())
  }
};
