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

#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>

#include <hdf5.h>

#include "H5Support/H5Macros.h"
#include "H5Support/H5Support.h"

/**
 * @brief Namespace to bring together some high level methods to read/write data to HDF5 files.
 * @author Mike Jackson
 * @date April 2020
 */
namespace H5Lite
{

constexpr size_t k_ChunkBase = 16 * 1024;
constexpr size_t k_ChunkMin = 8 * 1024;
constexpr size_t k_ChunkMax = 1024 * 1024;

/*-------------------------------------------------------------------------
 * Function: find_dataset
 *
 * Purpose: operator function used by H5LTfind_dataset
 *
 * Programmer: Pedro Vicente, pvn@ncsa.uiuc.edu
 *
 * Date: June 21, 2001
 *
 * Comments:
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
inline herr_t find_dataset(hid_t /*locationID*/, const char* name, void* op_data)
{
  H5SUPPORT_MUTEX_LOCK()
  /* Define a default zero value for return. This will cause the iterator to continue if
   * the dataset is not found yet.
   */

  int32_t returnError = 0;

  char* datasetName = reinterpret_cast<char*>(op_data);

  /* Shut the compiler up */
  // locationID=locationID;

  /* Define a positive value for return value if the dataset was found. This will
   * cause the iterator to immediately return that positive value,
   * indicating short-circuit success
   */

  if(std::strcmp(name, datasetName) == 0)
  {
    returnError = 1;
  }

  return returnError;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
inline herr_t find_attr(hid_t /*locationID*/, const char* name, const H5A_info_t* /*info*/, void* op_data)
{
  H5SUPPORT_MUTEX_LOCK()
  /* Define a default zero value for return. This will cause the iterator to continue if
   * the palette attribute is not found yet.
   */

  int32_t returnError = 0;

  char* attributeName = reinterpret_cast<char*>(op_data);

  /* Shut the compiler up */
  // locationID=locationID;

  /* Define a positive value for return value if the attribute was found. This will
   * cause the iterator to immediately return that positive value,
   * indicating short-circuit success
   */

  if(std::strcmp(name, attributeName) == 0)
  {
    returnError = 1;
  }

  return returnError;
}

template <class T, class Compare> inline constexpr const T& clamp(const T& v, const T& lo, const T& hi, Compare comp)
{
  return assert(!comp(hi, lo)), comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}

template <class T> inline constexpr const T& clamp(const T& v, const T& lo, const T& hi)
{
  return clamp(v, lo, hi, std::less<>());
}

/**
 * @brief Turns off the global error handler/reporting objects. Note that once
 * they are turned off using this method they CAN NOT be turned back on. If you
 * would like to turn them off for a piece of code then surround your code with
 * the HDF_ERROR_HANDLER_OFF and HDF_ERROR_HANDLER_ON macros defined in
 * H5Lite.h
 */
inline void disableErrorHandlers()
{
  H5SUPPORT_MUTEX_LOCK()

  HDF_ERROR_HANDLER_OFF
}

/**
 * @brief Opens an object for HDF5 operations
 * @param locationID The parent object that holds the true object we want to open
 * @param objectName The string name of the object
 * @param objectType The HDF5_TYPE of object
 * @return Standard HDF5 Error Conditions
 */
inline hid_t openId(hid_t locationID, const std::string& objectName, H5O_type_t objectType)
{
  H5SUPPORT_MUTEX_LOCK()

  hid_t objectID = -1;

  switch(objectType)
  {
  case H5O_TYPE_DATASET:

    /* Open the dataset. */
    if((objectID = H5Dopen(locationID, objectName.c_str(), H5P_DEFAULT)) < 0)
    {
      return -1;
    }
    break;

  case H5O_TYPE_GROUP:

    /* Open the group. */
    if((objectID = H5Gopen(locationID, objectName.c_str(), H5P_DEFAULT)) < 0)
    {
      return -1;
    }
    break;

  default:
    return -1;
  }

  return objectID;
}

/**
 * @brief Opens an HDF5 Object
 * @param objectID The Object id
 * @param objectType Basic Object Type
 * @return Standard HDF5 Error Conditions
 */
inline herr_t closeId(hid_t objectID, int32_t objectType)
{
  H5SUPPORT_MUTEX_LOCK()

  switch(objectType)
  {
  case H5O_TYPE_DATASET:
    /* Close the dataset. */
    if(H5Dclose(objectID) < 0)
    {
      return -1;
    }
    break;

  case H5O_TYPE_GROUP:
    /* Close the group. */
    if(H5Gclose(objectID) < 0)
    {
      return -1;
    }
    break;

  default:
    return -1;
  }

  return 1;
}

/**
 * @brief Returns a string of the HDF datatype
 * @param classType
 * @return
 */
inline std::string StringForHDFClassType(H5T_class_t classType)
{
  if(classType == H5T_NO_CLASS)
  {
    return "H5T_NO_CLASS";
  }
  if(classType == H5T_INTEGER)
  {
    return "H5T_INTEGER";
  }
  if(classType == H5T_FLOAT)
  {
    return "H5T_FLOAT";
  }
  if(classType == H5T_TIME)
  {
    return "H5T_TIME";
  }
  if(classType == H5T_STRING)
  {
    return "H5T_STRING";
  }
  if(classType == H5T_BITFIELD)
  {
    return "H5T_BITFIELD";
  }
  if(classType == H5T_OPAQUE)
  {
    return "H5T_OPAQUE";
  }
  if(classType == H5T_COMPOUND)
  {
    return "H5T_COMPOUND";
  }
  if(classType == H5T_REFERENCE)
  {
    return "H5T_REFERENCE";
  }
  if(classType == H5T_ENUM)
  {
    return "H5T_ENUM";
  }
  if(classType == H5T_VLEN)
  {
    return "H5T_VLEN";
  }
  if(classType == H5T_ARRAY)
  {
    return "H5T_ARRAY";
  }

  return "UNKNOWN";
}

/**
 * @brief Given one of the HDF Types as a string, this will return the HDF Type
 * as an hid_t value.
 * @param value The HDF_Type as a string
 * @return the hid_t value for the given type. -1 if the string does not match a type.
 */
inline hid_t HDFTypeFromString(const std::string& value)
{
  H5SUPPORT_MUTEX_LOCK()

  if(value == "H5T_STRING")
  {
    return H5T_STRING;
  }

  if(value == "H5T_NATIVE_INT8")
  {
    return H5T_NATIVE_INT8;
  }
  if(value == "H5T_NATIVE_UINT8")
  {
    return H5T_NATIVE_UINT8;
  }

  if(value == "H5T_NATIVE_INT16")
  {
    return H5T_NATIVE_INT16;
  }
  if(value == "H5T_NATIVE_UINT16")
  {
    return H5T_NATIVE_UINT16;
  }

  if(value == "H5T_NATIVE_INT32")
  {
    return H5T_NATIVE_INT32;
  }
  if(value == "H5T_NATIVE_UINT32")
  {
    return H5T_NATIVE_UINT32;
  }

  if(value == "H5T_NATIVE_INT64")
  {
    return H5T_NATIVE_INT64;
  }
  if(value == "H5T_NATIVE_UINT64")
  {
    return H5T_NATIVE_UINT64;
  }

  if(value == "H5T_NATIVE_FLOAT")
  {
    return H5T_NATIVE_FLOAT;
  }
  if(value == "H5T_NATIVE_DOUBLE")
  {
    return H5T_NATIVE_DOUBLE;
  }

  std::cout << "Error: HDFTypeFromString - Unknown Type: " << value << std::endl;
  return -1;
}

/**
 * @brief Returns a string version of the HDF Type
 * @param type The HDF5 Type to query
 * @return
 */
inline std::string StringForHDFType(hid_t dataTypeIdentifier)
{
  H5SUPPORT_MUTEX_LOCK()

  if(dataTypeIdentifier == H5T_STRING)
  {
    return "H5T_STRING";
  }

  if(H5Tequal(dataTypeIdentifier, H5T_NATIVE_INT8) != 0)
  {
    return "H5T_NATIVE_INT8";
  }
  if(H5Tequal(dataTypeIdentifier, H5T_NATIVE_UINT8) != 0)
  {
    return "H5T_NATIVE_UINT8";
  }

  if(H5Tequal(dataTypeIdentifier, H5T_NATIVE_INT16) != 0)
  {
    return "H5T_NATIVE_INT16";
  }
  if(H5Tequal(dataTypeIdentifier, H5T_NATIVE_UINT16) != 0)
  {
    return "H5T_NATIVE_UINT16";
  }

  if(H5Tequal(dataTypeIdentifier, H5T_NATIVE_INT32) != 0)
  {
    return "H5T_NATIVE_INT32";
  }
  if(H5Tequal(dataTypeIdentifier, H5T_NATIVE_UINT32) != 0)
  {
    return "H5T_NATIVE_UINT32";
  }

  if(H5Tequal(dataTypeIdentifier, H5T_NATIVE_INT64) != 0)
  {
    return "H5T_NATIVE_INT64";
  }
  if(H5Tequal(dataTypeIdentifier, H5T_NATIVE_UINT64) != 0)
  {
    return "H5T_NATIVE_UINT64";
  }

  if(H5Tequal(dataTypeIdentifier, H5T_NATIVE_FLOAT) != 0)
  {
    return "H5T_NATIVE_FLOAT";
  }
  if(H5Tequal(dataTypeIdentifier, H5T_NATIVE_DOUBLE) != 0)
  {
    return "H5T_NATIVE_DOUBLE";
  }

  std::cout << "Error: HDFTypeForPrimitiveAsStr - Unknown Type: " << dataTypeIdentifier << std::endl;
  return "Unknown";
}

/**
 * @brief Returns the HDF Type for a given primitive value.
 * @param value A value to use. Can be anything. Just used to get the type info
 * from
 * @return A std::string representing the HDF5 Type
 */
template <typename T> inline std::string HDFTypeForPrimitiveAsStr(T value)
{
  H5SUPPORT_MUTEX_LOCK()

  if(std::is_same<T, int8_t>::value)
  {
    return "H5T_NATIVE_INT8";
  }
  if(std::is_same<T, uint8_t>::value)
  {
    return "H5T_NATIVE_UINT8";
  }

  if(std::is_same<T, int16_t>::value)
  {
    return "H5T_NATIVE_INT16";
  }
  if(std::is_same<T, uint16_t>::value)
  {
    return "H5T_NATIVE_UINT16";
  }

  if(std::is_same<T, int32_t>::value)
  {
    return "H5T_NATIVE_INT32";
  }
  if(std::is_same<T, uint32_t>::value)
  {
    return "H5T_NATIVE_UINT32";
  }

  if(std::is_same<T, int64_t>::value)
  {
    return "H5T_NATIVE_INT64";
  }
  if(std::is_same<T, uint64_t>::value)
  {
    return "H5T_NATIVE_UINT64";
  }

  if(std::is_same<T, float>::value)
  {
    return "H5T_NATIVE_FLOAT";
  }
  if(std::is_same<T, double>::value)
  {
    return "H5T_NATIVE_DOUBLE";
  }

  // if (typeid(value) == typeid(bool)) return "H5T_NATIVE_UINT8";

  std::cout << "Error: HDFTypeForPrimitiveAsStr - Unknown Type: " << typeid(value).name() << std::endl;
  return "";
}

/**
 * @brief Returns the HDF Type for a given primitive value.
 * @param value A value to use. Can be anything. Just used to get the type info
 * from
 * @return The HDF5 native type for the value
 */
template <typename T> inline hid_t HDFTypeForPrimitive(T value)
{
  H5SUPPORT_MUTEX_LOCK()

  if(std::is_same<T, float>::value)
  {
    return H5T_NATIVE_FLOAT;
  }
  if(std::is_same<T, double>::value)
  {
    return H5T_NATIVE_DOUBLE;
  }

  if(std::is_same<T, int8_t>::value)
  {
    return H5T_NATIVE_INT8;
  }
  if(std::is_same<T, uint8_t>::value)
  {
    return H5T_NATIVE_UINT8;
  }
#if CMP_TYPE_CHAR_IS_SIGNED
  if(std::is_same<T, char>::value)
  {
    return H5T_NATIVE_INT8;
  }
#else
  if(std::is_same<T, char>::value)
  {
    return H5T_NATIVE_UINT8;
  }
#endif
  if(std::is_same<T, signed char>::value)
  {
    return H5T_NATIVE_INT8;
  }
  if(std::is_same<T, unsigned char>::value)
  {
    return H5T_NATIVE_UINT8;
  }

  if(std::is_same<T, int16_t>::value)
  {
    return H5T_NATIVE_INT16;
  }
  if(std::is_same<T, short>::value)
  {
    return H5T_NATIVE_INT16;
  }
  if(std::is_same<T, signed short>::value)
  {
    return H5T_NATIVE_INT16;
  }
  if(std::is_same<T, uint16_t>::value)
  {
    return H5T_NATIVE_UINT16;
  }
  if(std::is_same<T, unsigned short>::value)
  {
    return H5T_NATIVE_UINT16;
  }

  if(std::is_same<T, int32_t>::value)
  {
    return H5T_NATIVE_INT32;
  }
  if(std::is_same<T, uint32_t>::value)
  {
    return H5T_NATIVE_UINT32;
  }
#if(CMP_SIZEOF_INT == 4)
  if(std::is_same<T, int>::value)
  {
    return H5T_NATIVE_INT32;
  }
  if(std::is_same<T, signed int>::value)
  {
    return H5T_NATIVE_INT32;
  }
  if(std::is_same<T, unsigned int>::value)
  {
    return H5T_NATIVE_UINT32;
  }
#endif

#if(CMP_SIZEOF_LONG == 4)
  if(std::is_same<T, long int>::value)
  {
    return H5T_NATIVE_INT32;
  }
  if(std::is_same<T, signed long int>::value)
  {
    return H5T_NATIVE_INT32;
  }
  if(std::is_same<T, unsigned long int>::value)
  {
    return H5T_NATIVE_UINT32;
  }
#elif(CMP_SIZEOF_LONG == 8)
  if(std::is_same<T, long int>::value)
  {
    return H5T_NATIVE_INT64;
  }
  if(std::is_same<T, signed long int>::value)
  {
    return H5T_NATIVE_INT64;
  }
  if(std::is_same<T, unsigned long int>::value)
  {
    return H5T_NATIVE_UINT64;
  }
#endif

#if(CMP_SIZEOF_LONG_LONG == 8)
  if(std::is_same<T, long long int>::value)
  {
    return H5T_NATIVE_INT64;
  }
  if(std::is_same<T, signed long long int>::value)
  {
    return H5T_NATIVE_INT64;
  }
  if(std::is_same<T, unsigned long long int>::value)
  {
    return H5T_NATIVE_UINT64;
  }
#endif
  if(std::is_same<T, int64_t>::value)
  {
    return H5T_NATIVE_INT64;
  }
  if(std::is_same<T, uint64_t>::value)
  {
    return H5T_NATIVE_UINT64;
  }

  if(std::is_same<T, bool>::value)
  {
    return H5T_NATIVE_UINT8;
  }

  std::cout << "Error: HDFTypeForPrimitive - Unknown Type: " << (typeid(value).name()) << std::endl;
  const char* name = typeid(value).name();
  if(nullptr != name && name[0] == 'l')
  {
    std::cout << "You are using 'long int' as a type which is not 32/64 bit safe. Suggest you use one of the H5SupportTypes defined in <Common/H5SupportTypes.h> such as int32_t or uint32_t."
              << std::endl;
  }
  return -1;
}

/**
 * @brief Inquires if an attribute named attributeName exists attached to the object locationID.
 * @param locationID The location to search
 * @param attributeName The attribute to search for
 * @return Standard HDF5 Error condition
 */
inline herr_t findAttribute(hid_t locationID, const std::string& attributeName)
{
  H5SUPPORT_MUTEX_LOCK()

  hsize_t attributeNum;
  herr_t returnError = 0;

  attributeNum = 0;
  returnError = H5Aiterate(locationID, H5_INDEX_NAME, H5_ITER_INC, &attributeNum, find_attr, const_cast<char*>(attributeName.c_str()));

  return returnError;
}

/**
 * @brief Finds a Data set given a data set name
 * @param locationID The location to search
 * @param datasetName The dataset to search for
 * @return Standard HDF5 Error condition. Negative=DataSet
 */
inline bool datasetExists(hid_t locationID, const std::string& datasetName)
{
  H5SUPPORT_MUTEX_LOCK()

  H5O_info_t objectInfo{};
  HDF_ERROR_HANDLER_OFF
  herr_t error = H5Oget_info_by_name(locationID, datasetName.c_str(), &objectInfo, H5P_DEFAULT);
  HDF_ERROR_HANDLER_ON
  return error >= 0;
}

/**
 * @brief Writes the data of a pointer to an HDF5 file
 * @param locationID The hdf5 object id of the parent
 * @param datasetName The name of the dataset to write to. This can be a name of Path
 * @param rank The number of dimensions
 * @param dims The sizes of each dimension
 * @param data The data to be written.
 * @return Standard hdf5 error condition.
 */
template <typename T> inline herr_t writePointerDataset(hid_t locationID, const std::string& datasetName, int32_t rank, const hsize_t* dims, const T* data)
{
  H5SUPPORT_MUTEX_LOCK()

  herr_t error = -1;
  hid_t datasetID = -1;
  hid_t dataspaceID = -1;
  herr_t returnError = 0;

  if(nullptr == data)
  {
    return -2;
  }
  hid_t dataType = HDFTypeForPrimitive(data[0]);
  if(dataType == -1)
  {
    return -1;
  }
  // Create the DataSpace
  dataspaceID = H5Screate_simple(rank, dims, nullptr);
  if(dataspaceID < 0)
  {
    return static_cast<herr_t>(dataspaceID);
  }
  // Create the Dataset
  // This will fail if datasetName contains a "/"!
  datasetID = H5Dcreate(locationID, datasetName.c_str(), dataType, dataspaceID, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  if(datasetID >= 0)
  {
    error = H5Dwrite(datasetID, dataType, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    if(error < 0)
    {
      std::cout << "Error Writing Data '" << datasetName << "'" << std::endl;
      std::cout << "    rank = " << rank << std::endl;
      uint64_t totalSize = 1;
      for(size_t i = 0; i < rank; ++i)
      {
        std::cout << "    dim[" << i << "] = " << dims[i] << std::endl;
        totalSize = totalSize * dims[i];
      }
      std::cout << "    Total Elements = " << totalSize << std::endl;
      std::cout << "    Size of Type (Bytes) = " << sizeof(T) << std::endl;
      std::cout << "    Total Bytes to Write =  " << (sizeof(T) * totalSize) << std::endl;
      returnError = error;
    }
    error = H5Dclose(datasetID);
    if(error < 0)
    {
      std::cout << "Error Closing Dataset." << std::endl;
      returnError = error;
    }
  }
  else
  {
    returnError = static_cast<herr_t>(datasetID);
  }
  /* Terminate access to the data space. */
  error = H5Sclose(dataspaceID);
  if(error < 0)
  {
    std::cout << "Error Closing Dataspace" << std::endl;
    returnError = error;
  }
  return returnError;
}

/**
 * @brief Replaces the given dataset with the data of a pointer to an HDF5 file. Creates the dataset if it does not exist.
 * @param locationID The hdf5 object id of the parent
 * @param datasetName The name of the dataset to write to. This can be a name of Path
 * @param rank The number of dimensions
 * @param dims The sizes of each dimension
 * @param data The data to be written.
 * @return Standard hdf5 error condition.
 */
template <typename T> inline herr_t replacePointerDataset(hid_t locationID, const std::string& datasetName, int32_t rank, const hsize_t* dims, const T* data)
{
  H5SUPPORT_MUTEX_LOCK()

  herr_t error = -1;
  hid_t datasetID = -1;
  hid_t dataspaceID = -1;
  herr_t returnError = 0;

  if(data == nullptr)
  {
    return -2;
  }

  hid_t dataType = H5Lite::HDFTypeForPrimitive(data[0]);
  if(dataType == -1)
  {
    return -1;
  }
  // Create the DataSpace
  dataspaceID = H5Screate_simple(rank, dims, nullptr);
  if(dataspaceID < 0)
  {
    return dataspaceID;
  }

  HDF_ERROR_HANDLER_OFF
  datasetID = H5Dopen(locationID, datasetName.c_str(), H5P_DEFAULT);
  HDF_ERROR_HANDLER_ON
  if(datasetID < 0) // dataset does not exist so create it
  {
    datasetID = H5Dcreate(locationID, datasetName.c_str(), dataType, dataspaceID, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  }
  if(datasetID >= 0)
  {
    error = H5Dwrite(datasetID, dataType, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    if(error < 0)
    {
      std::cout << "Error Writing Data" << std::endl;
      returnError = error;
    }
    error = H5Dclose(datasetID);
    if(error < 0)
    {
      std::cout << "Error Closing Dataset." << std::endl;
      returnError = error;
    }
  }
  else
  {
    returnError = static_cast<herr_t>(datasetID);
  }
  /* Terminate access to the data space. */
  error = H5Sclose(dataspaceID);
  if(error < 0)
  {
    std::cout << "Error Closing Dataspace" << std::endl;
    returnError = error;
  }
  return returnError;
}

/**
 * @brief Creates a Dataset with the given name at the location defined by locationID
 *
 *
 * @param locationID The Parent location to store the data
 * @param datasetName The name of the dataset
 * @param dims The dimensions of the dataset
 * @param data The data to write to the file
 * @return Standard HDF5 error conditions
 *
 * The dimensions of the data sets are usually passed as both a "rank" and
 * dimensions array. By using a std::vector<hsize_t> that stores the values of
 * each of the dimensions we can reduce the number of arguments to this method as
 * the value of the "rank" simply becomes dims.length(). So to create a Dims variable
 * for a 3D data space of size(x,y,z) = {10,20,30} I would use the following code:
 * <code>
 * std::vector<hsize_t> dims;
 * dims.push_back(10);
 * dims.push_back(20);
 * dims.push_back(30);
 * </code>
 *
 * Also when passing data BE SURE that the type of data and the data type match.
 * For example if I create some data in a std::vector<UInt8Type> I would need to
 * pass H5T_NATIVE_UINT8 as the dataType.
 */
template <typename T> inline herr_t writeVectorDataset(hid_t locationID, const std::string& datasetName, const std::vector<hsize_t>& dims, const std::vector<T>& data)
{
  return writePointerDataset(locationID, datasetName, static_cast<int32_t>(dims.size()), dims.data(), data.data());
}

/**
 * @brief Returns a guess for the vector of chunk dimensions based on the input parameters.
 * @param dims The vector dimensions of the dataset
 * @param typeSize The size of the data type for the dataset
 * @return The vector of chunk dimensions guess
 */
inline std::vector<hsize_t> guessChunkSize(const std::vector<hsize_t>& dims, size_t typeSize)
{
  std::vector<hsize_t> chunks(dims.cbegin(), dims.cend());
  size_t chunksSize = chunks.size();

  hsize_t product = std::accumulate(chunks.cbegin(), chunks.cend(), static_cast<hsize_t>(1), std::multiplies<hsize_t>());
  hsize_t datasetSize = product * typeSize;
  double percentage = std::pow(2.0, std::log10(static_cast<double>(datasetSize) / (1024.0 * 1024.0)));
  hsize_t targetSize = static_cast<hsize_t>(static_cast<double>(k_ChunkBase) * percentage);
  targetSize = clamp(targetSize, static_cast<hsize_t>(k_ChunkMin), static_cast<hsize_t>(k_ChunkMax));

  size_t index = 0;

  bool foundChunkSize = false;

  while(!foundChunkSize)
  {
    product = std::accumulate(chunks.cbegin(), chunks.cend(), static_cast<hsize_t>(1), std::multiplies<hsize_t>());
    hsize_t chunkBytes = product * typeSize;
    if(chunkBytes < targetSize)
    {
      foundChunkSize = true;
      break;
    }

    if(chunkBytes < k_ChunkMax && static_cast<double>(chunkBytes - targetSize) / static_cast<double>(targetSize) < 0.5)
    {
      foundChunkSize = true;
      break;
    }

    if(product == 1)
    {
      foundChunkSize = true;
      break;
    }

    size_t i = index % chunksSize;

    chunks[i] = static_cast<hsize_t>(std::ceil(static_cast<double>(chunks[i]) / 2.0));
    ++index;
  }

  return chunks;
}

/**
 * @brief Returns a guess for the vector of chunk dimensions based on the input parameters.
 * @param rank The number of dimensions
 * @param dims The dimensions of the dataset
 * @param typeSize The size of the data type for the dataset
 * @return The vector of chunk dimensions guess
 */
inline std::vector<hsize_t> guessChunkSize(int32_t rank, const hsize_t* dims, size_t typeSize)
{
  std::vector<hsize_t> vDims(rank, 0);
  std::copy(dims, dims + rank, vDims.data());
  return guessChunkSize(vDims, typeSize);
}

#ifdef H5_HAVE_FILTER_DEFLATE
/**
 * @brief Creates a Dataset with the given name at the location defined by locationID with the given compression
 *
 * @param locationID The Parent location to store the data
 * @param datasetName The name of the dataset
 * @param rank The number of dimensions
 * @param dims The dimensions of the dataset
 * @param data The data to write to the file
 * @param cRank The number of dimensions for cDims
 * @param cDims The chunk dimensions
 * @param compressionLevel The compression level (0-9)
 * @return Standard HDF5 error conditions
 */
template <typename T>
inline herr_t writePointerDatasetCompressed(hid_t locationID, const std::string& datasetName, int32_t rank, const hsize_t* dims, const T* data, int32_t cRank, const hsize_t* cDims,
                                            int32_t compressionLevel)
{
  H5SUPPORT_MUTEX_LOCK()

  herr_t error = -1;
  herr_t returnError = 0;

  if(data == nullptr)
  {
    return -100;
  }

  hid_t dataType = HDFTypeForPrimitive(data[0]);

  if(dataType == -1)
  {
    return -101;
  }

  // Create the DataSpace

  hid_t dataspaceID = H5Screate_simple(rank, dims, nullptr);
  if(dataspaceID < 0)
  {
    return -102;
  }

  // Create property list for chunking and compression

  hid_t propertListID = H5Pcreate(H5P_DATASET_CREATE);
  if(propertListID < 0)
  {
    returnError = -103;
    error = H5Sclose(dataspaceID);
    if(error < 0)
    {
      returnError = -104;
    }
    return returnError;
  }

  error = H5Pset_chunk(propertListID, cRank, cDims);
  if(error < 0)
  {
    error = H5Pclose(propertListID);
    if(error < 0)
    {
      returnError = -105;
    }
    error = H5Sclose(dataspaceID);
    if(error < 0)
    {
      returnError = -106;
    }
    return returnError;
  }

  error = H5Pset_deflate(propertListID, compressionLevel);
  if(error < 0)
  {
    error = H5Pclose(propertListID);
    if(error < 0)
    {
      returnError = -107;
    }
    error = H5Sclose(dataspaceID);
    if(error < 0)
    {
      returnError = -108;
    }
    return returnError;
  }

  // Create the Dataset

  hid_t datasetID = H5Dcreate(locationID, datasetName.c_str(), dataType, dataspaceID, H5P_DEFAULT, propertListID, H5P_DEFAULT);
  if(datasetID >= 0)
  {
    error = H5Dwrite(datasetID, dataType, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    if(error < 0)
    {
      std::cout << "Error Writing Data" << std::endl;
      returnError = -108;
    }
    error = H5Dclose(datasetID);
    if(error < 0)
    {
      std::cout << "Error Closing Dataset." << std::endl;
      returnError = -110;
    }
  }
  else
  {
    returnError = -111;
  }

  // Terminate access to the data space and property list.

  error = H5Pclose(propertListID);
  if(error < 0)
  {
    std::cout << "Error Closing Property List" << std::endl;
    returnError = -112;
  }
  error = H5Sclose(dataspaceID);
  if(error < 0)
  {
    std::cout << "Error Closing Dataspace" << std::endl;
    returnError = -113;
  }

  return returnError;
}

/**
 * @brief Creates a Dataset with the given name at the location defined by locationID with the given compression
 *
 * @param locationID The Parent location to store the data
 * @param datasetName The name of the dataset
 * @param dims The dimensions of the dataset
 * @param data The data to write to the file
 * @param cDims The chunk dimensions
 * @param compressionLevel The compression level (0-9)
 * @return Standard HDF5 error conditions
 */
template <typename T>
inline herr_t writeVectorDatasetCompressed(hid_t locationID, const std::string& datasetName, const std::vector<hsize_t>& dims, const std::vector<T>& data, const std::vector<hsize_t>& cDims,
                                           int32_t compressionLevel)
{
  return writePointerDatasetCompressed(locationID, datasetName, static_cast<int32_t>(dims.size()), dims.data(), data.data(), static_cast<int32_t>(cDims.size()), cDims.data(), compressionLevel);
}
#endif

/**
 * @brief Creates a Dataset with the given name at the location defined by locationID from a std::array
 *
 * @param locationID The Parent location to store the data
 * @param datasetName The name of the dataset
 * @param dims The dimensions of the dataset
 * @param data The data to write to the file
 * @return Standard HDF5 error conditions
 */
template <typename T, size_t _Size> inline herr_t writeArrayDataset(hid_t locationID, const std::string& datasetName, const std::vector<hsize_t>& dims, const std::array<T, _Size>& data)
{
  return writePointerDataset(locationID, datasetName, static_cast<int32_t>(dims.size()), dims.data(), data.data());
}

/**
 * @brief Creates a Dataset with the given name at the location defined by locationID.
 * This version of writeDataset should be used with a single scalar value. If you
 * need to write an array of values, use the form that takes an std::vector<>
 *
 * @param locationID The Parent location to store the data
 * @param datasetName The name of the dataset
 * @param value The value to write to the HDF5 dataset
 * @return Standard HDF5 error conditions
 */
template <typename T> inline herr_t writeScalarDataset(hid_t locationID, const std::string& datasetName, const T& value)
{
  H5SUPPORT_MUTEX_LOCK()

  herr_t error = -1;
  hid_t datasetID = -1;
  hid_t dataspaceID = -1;
  herr_t returnError = 0;
  hsize_t dims = 1;
  hid_t rank = 1;
  hid_t dataType = HDFTypeForPrimitive(value);
  if(dataType == -1)
  {
    return -1;
  }
  // Create the DataSpace
  dataspaceID = H5Screate_simple(static_cast<int>(rank), &(dims), nullptr);
  if(dataspaceID < 0)
  {
    return static_cast<herr_t>(dataspaceID);
  }
  // Create the Dataset
  datasetID = H5Dcreate(locationID, datasetName.c_str(), dataType, dataspaceID, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  if(datasetID >= 0)
  {
    error = H5Dwrite(datasetID, dataType, H5S_ALL, H5S_ALL, H5P_DEFAULT, &value);
    if(error < 0)
    {
      std::cout << "Error Writing Data" << std::endl;
      returnError = error;
    }
    error = H5Dclose(datasetID);
    if(error < 0)
    {
      std::cout << "Error Closing Dataset." << std::endl;
      returnError = error;
    }
  }
  else
  {
    returnError = static_cast<herr_t>(datasetID);
  }
  /* Terminate access to the data space. */
  error = H5Sclose(dataspaceID);
  if(error < 0)
  {
    std::cout << "Error Closing Dataspace" << std::endl;
    returnError = error;
  }
  return returnError;
}

/**
 * @brief Writes a std::string as a HDF Dataset.
 * @param locationID The Parent location to write the dataset
 * @param datasetName The Name to use for the dataset
 * @param data The actual data to write as a null terminated string
 * @return Standard HDF5 error conditions
 */
inline herr_t writeStringDataset(hid_t locationID, const std::string& datasetName, const std::string& data)
{
  H5SUPPORT_MUTEX_LOCK()

  hid_t datasetID = -1;
  hid_t dataspaceID = -1;
  hid_t typeID = -1;
  size_t size = 0;
  herr_t error = -1;
  herr_t returnError = 0;

  /* create a string data type */
  if((typeID = H5Tcopy(H5T_C_S1)) >= 0)
  {
    size = data.size() + 1;
    if(H5Tset_size(typeID, size) >= 0)
    {
      if(H5Tset_strpad(typeID, H5T_STR_NULLTERM) >= 0)
      {
        /* Create the data space for the dataset. */
        if((dataspaceID = H5Screate(H5S_SCALAR)) >= 0)
        {
          /* Create or open the dataset. */
          HDF_ERROR_HANDLER_OFF
          datasetID = H5Dopen(locationID, datasetName.c_str(), H5P_DEFAULT);
          HDF_ERROR_HANDLER_ON
          if(datasetID < 0) // dataset does not exist so create it
          {
            datasetID = H5Dcreate(locationID, datasetName.c_str(), typeID, dataspaceID, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
          }

          if(datasetID >= 0)
          {
            if(!data.empty())
            {
              error = H5Dwrite(datasetID, typeID, H5S_ALL, H5S_ALL, H5P_DEFAULT, data.c_str());
              if(error < 0)
              {
                std::cout << "Error Writing String Data" << std::endl;
                returnError = error;
              }
            }
          }
          else
          {
            // returnError = datasetID;
            returnError = 0;
          }
          CloseH5D(datasetID, error, returnError, datasetName);
          //          error = H5Dclose(datasetID);
          //          if (error < 0) {
          //            std::cout << "Error Closing Dataset." << std::endl;
          //            returnError = error;
          //          }
        }
        CloseH5S(dataspaceID, error, returnError);
        //        error = H5Sclose(dataspaceID);
        //        if ( error < 0) {
        //          std::cout << "Error closing Dataspace." << std::endl;
        //          returnError = error;
        //        }
      }
    }
    CloseH5T(typeID, error, returnError);
    //    error = H5Tclose(typeID);
    //    if (error < 0 ) {
    //     std::cout << "Error closing DataType" << std::endl;
    //     returnError = error;
    //    }
  }
  return returnError;
}

/**
 * @brief Writes a null terminated 'C String' to an HDF Dataset.
 * @param locationID The Parent location to write the dataset
 * @param datasetName The Name to use for the dataset
 * @param size The number of characters in the string
 * @param data const char pointer to write as a null terminated string
 * @return Standard HDF5 error conditions
 */
inline herr_t writeStringDataset(hid_t locationID, const std::string& datasetName, size_t size, const char* data)
{
  H5SUPPORT_MUTEX_LOCK()

  hid_t datasetID = -1;
  hid_t dataspaceID = -1;
  hid_t typeID = -1;
  herr_t error = -1;
  herr_t returnError = 0;

  /* create a string data type */
  if((typeID = H5Tcopy(H5T_C_S1)) >= 0)
  {
    if(H5Tset_size(typeID, size) >= 0)
    {
      if(H5Tset_strpad(typeID, H5T_STR_NULLTERM) >= 0)
      {
        /* Create the data space for the dataset. */
        if((dataspaceID = H5Screate(H5S_SCALAR)) >= 0)
        {
          /* Create the dataset. */
          if((datasetID = H5Dcreate(locationID, datasetName.c_str(), typeID, dataspaceID, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) >= 0)
          {
            if(nullptr != data)
            {
              error = H5Dwrite(datasetID, typeID, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
              if(error < 0)
              {
                std::cout << "Error Writing String Data" << std::endl;
                returnError = error;
              }
            }
          }
          else
          {
            // returnError = datasetID;
            returnError = 0;
          }
          CloseH5D(datasetID, error, returnError, datasetName);
        }
        CloseH5S(dataspaceID, error, returnError);
      }
    }
    CloseH5T(typeID, error, returnError);
  }
  return returnError;
}

/**
 * @brief Writes a vector of null terminated strings to an HDF dataset.
 * @param locationID
 * @param datasetName
 * @param size
 * @param data
 * @return
 */
inline herr_t writeVectorOfStringsDataset(hid_t locationID, const std::string& datasetName, const std::vector<std::string>& data)
{
  H5SUPPORT_MUTEX_LOCK()

  hid_t dataspaceID = -1;
  hid_t memSpace = -1;
  hid_t datatype = -1;
  hid_t datasetID = -1;
  herr_t error = -1;
  herr_t returnError = 0;

  std::array<hsize_t, 1> dims = {data.size()};
  if((dataspaceID = H5Screate_simple(static_cast<int>(dims.size()), dims.data(), nullptr)) >= 0)
  {
    dims[0] = 1;

    if((memSpace = H5Screate_simple(static_cast<int>(dims.size()), dims.data(), nullptr)) >= 0)
    {

      datatype = H5Tcopy(H5T_C_S1);
      H5Tset_size(datatype, H5T_VARIABLE);

      if((datasetID = H5Dcreate(locationID, datasetName.c_str(), datatype, dataspaceID, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) >= 0)
      {
        // Select the "memory" to be written out - just 1 record.
        hsize_t offset[] = {0};
        hsize_t count[] = {1};
        H5Sselect_hyperslab(memSpace, H5S_SELECT_SET, offset, nullptr, count, nullptr);
        hsize_t pos = 0;
        for(const auto& element : data)
        {
          // Select the file position, 1 record at position 'pos'
          hsize_t count[] = {1};
          hsize_t offset[] = {pos};
          pos++;
          H5Sselect_hyperslab(dataspaceID, H5S_SELECT_SET, offset, nullptr, count, nullptr);
          const char* strPtr = element.c_str();
          error = H5Dwrite(datasetID, datatype, memSpace, dataspaceID, H5P_DEFAULT, &strPtr);
          if(error < 0)
          {
            std::cout << "Error Writing String Data: " __FILE__ << "(" << __LINE__ << ")" << std::endl;
            returnError = error;
          }
        }
        CloseH5D(datasetID, error, returnError, datasetName);
      }
      H5Tclose(datatype);
      CloseH5S(memSpace, error, returnError);
    }

    CloseH5S(dataspaceID, error, returnError);
  }
  return returnError;
}

/**
 * @brief Writes an Attribute to an HDF5 Object
 * @param locationID The Parent Location of the HDFobject that is getting the attribute
 * @param objectName The Name of Object to write the attribute into.
 * @param attributeName The Name of the Attribute
 * @param rank The number of dimensions in the attribute data
 * @param dims The Dimensions of the attribute data
 * @param data The Attribute Data to write as a pointer
 * @return Standard HDF Error Condition
 */
template <typename T> inline herr_t writePointerAttribute(hid_t locationID, const std::string& objectName, const std::string& attributeName, int32_t rank, const hsize_t* dims, const T* data)
{
  H5SUPPORT_MUTEX_LOCK()

  hid_t objectID, dataspaceID, attributeID;
  herr_t hasAttribute;
  H5O_info_t objectInfo;
  herr_t error = 0;
  herr_t returnError = 0;
  T test = 0x00;
  hid_t dataType = HDFTypeForPrimitive(test);
  if(dataType == -1)
  {
    std::cout << "dataType was unknown" << std::endl;
    return -1;
  }
  /* Get the type of object */

  if(H5Oget_info_by_name(locationID, objectName.c_str(), &objectInfo, H5P_DEFAULT) < 0)
  {
    std::cout << "Error getting object info at locationID (" << locationID << ") with object name (" << objectName << ")" << std::endl;
    return -1;
  }
  /* Open the object */
  objectID = openId(locationID, objectName, objectInfo.type);
  if(objectID < 0)
  {
    std::cout << "Error opening Object for Attribute operations." << std::endl;
    return -1;
  }

  dataspaceID = H5Screate_simple(rank, dims, nullptr);
  if(dataspaceID >= 0)
  {
    /* Verify if the attribute already exists */
    hasAttribute = findAttribute(objectID, attributeName);

    /* The attribute already exists, delete it */
    if(hasAttribute == 1)
    {
      error = H5Adelete(objectID, attributeName.c_str());
      if(error < 0)
      {
        std::cout << "Error Deleting Existing Attribute" << std::endl;
        returnError = error;
      }
    }

    if(error >= 0)
    {
      /* Create the attribute. */
      attributeID = H5Acreate(objectID, attributeName.c_str(), dataType, dataspaceID, H5P_DEFAULT, H5P_DEFAULT);
      if(attributeID >= 0)
      {
        /* Write the attribute data. */
        error = H5Awrite(attributeID, dataType, data);
        if(error < 0)
        {
          std::cout << "Error Writing Attribute" << std::endl;
          returnError = error;
        }
      }
      /* Close the attribute. */
      error = H5Aclose(attributeID);
      if(error < 0)
      {
        std::cout << "Error Closing Attribute" << std::endl;
        returnError = error;
      }
    }
    /* Close the dataspace. */
    error = H5Sclose(dataspaceID);
    if(error < 0)
    {
      std::cout << "Error Closing Dataspace" << std::endl;
      returnError = error;
    }
  }
  else
  {
    returnError = static_cast<herr_t>(dataspaceID);
  }
  /* Close the object */
  error = closeId(objectID, objectInfo.type);
  if(error < 0)
  {
    std::cout << "Error Closing HDF5 Object ID" << std::endl;
    returnError = error;
  }
  return returnError;
}

/**
 * @brief Writes an Attribute to an HDF5 Object
 * @param locationID The Parent Location of the HDFobject that is getting the attribute
 * @param objectName The Name of Object to write the attribute into.
 * @param attributeName The Name of the Attribute
 * @param dims The Dimensions of the data set
 * @param data The Attribute Data to write
 * @return Standard HDF Error Condition
 */
template <typename T>
inline herr_t writeVectorAttribute(hid_t locationID, const std::string& objectName, const std::string& attributeName, const std::vector<hsize_t>& dims, const std::vector<T>& data)
{
  return writePointerAttribute(locationID, objectName, attributeName, static_cast<int32_t>(dims.size()), dims.data(), data.data());
}

/**
 * @brief Writes a null terminated string as an attribute
 * @param locationID The location to look for objectName
 * @param objectName The Object to write the attribute to
 * @param attributeName The name of the Attribute
 * @param size The number of characters  in the string
 * @param data pointer to a const char array
 * @return Standard HDF error conditions
 */
inline herr_t writeStringAttribute(hid_t locationID, const std::string& objectName, const std::string& attributeName, hsize_t size, const char* data)
{
  H5SUPPORT_MUTEX_LOCK()

  hid_t attributeType;
  hid_t attributeSpaceID;
  hid_t attributeID;
  hid_t objectID;
  int32_t hasAttribute;
  H5O_info_t objectInfo{};
  size_t attributeSize;
  herr_t error = 0;
  herr_t returnError = 0;

  /* Get the type of object */
  returnError = H5Oget_info_by_name(locationID, objectName.c_str(), &objectInfo, H5P_DEFAULT);
  if(returnError >= 0)
  {
    /* Open the object */
    objectID = openId(locationID, objectName, objectInfo.type);
    if(objectID >= 0)
    {
      /* Create the attribute */
      attributeType = H5Tcopy(H5T_C_S1);
      if(attributeType >= 0)
      {
        attributeSize = size; /* extra null term */
        error = H5Tset_size(attributeType, attributeSize);
        if(error < 0)
        {
          std::cout << "Error Setting H5T Size" << std::endl;
          returnError = error;
        }
        if(error >= 0)
        {
          error = H5Tset_strpad(attributeType, H5T_STR_NULLTERM);
          if(error < 0)
          {
            std::cout << "Error adding a null terminator." << std::endl;
            returnError = error;
          }
          if(error >= 0)
          {
            attributeSpaceID = H5Screate(H5S_SCALAR);
            if(attributeSpaceID >= 0)
            {
              /* Verify if the attribute already exists */
              hasAttribute = findAttribute(objectID, attributeName);
              /* The attribute already exists, delete it */
              if(hasAttribute == 1)
              {
                error = H5Adelete(objectID, attributeName.c_str());
                if(error < 0)
                {
                  std::cout << "Error Deleting Attribute '" << attributeName << "' from Object '" << objectName << "'" << std::endl;
                  returnError = error;
                }
              }
              if(error >= 0)
              {
                /* Create and write the attribute */
                attributeID = H5Acreate(objectID, attributeName.c_str(), attributeType, attributeSpaceID, H5P_DEFAULT, H5P_DEFAULT);
                if(attributeID >= 0)
                {
                  error = H5Awrite(attributeID, attributeType, data);
                  if(error < 0)
                  {
                    std::cout << "Error Writing String attribute." << std::endl;

                    returnError = error;
                  }
                }
                CloseH5A(attributeID, error, returnError);
              }
              CloseH5S(attributeSpaceID, error, returnError);
            }
          }
        }
        CloseH5T(attributeType, error, returnError);
      }
      else
      {
        // returnError = attributeType;
      }
      /* Close the object */
      error = closeId(objectID, objectInfo.type);
      if(error < 0)
      {
        std::cout << "Error Closing Object Id" << std::endl;
        returnError = error;
      }
    }
  }
  return returnError;
}

/**
 * @brief Writes a string as a null terminated attribute.
 * @param locationID The location to look for objectName
 * @param objectName The Object to write the attribute to
 * @param attributeName The name of the Attribute
 * @param data The string to write as the attribute
 * @return Standard HDF error conditions
 */
inline herr_t writeStringAttribute(hid_t locationID, const std::string& objectName, const std::string& attributeName, const std::string& data)
{
  return writeStringAttribute(locationID, objectName, attributeName, data.size() + 1, data.data());
}

/**
 * @brief Writes attributes that all have a data type of STRING. The first value
 * in each set is the key, the second is the actual value of the attribute.
 * @param locationID The location to look for objectName
 * @param objectName The Object to write the attribute to
 * @param attributes The attributes to be written where the first value is the name
 * of the attribute, and the second is the actual value of the attribute.
 * @return Standard HDF error condition
 */
inline herr_t writeStringAttributes(hid_t locationID, const std::string& objectName, const std::map<std::string, std::string>& attributes)
{
  H5SUPPORT_MUTEX_LOCK()

  herr_t error = 0;
  for(const auto& attribute : attributes)
  {
    error = writeStringAttribute(locationID, objectName, attribute.first, attribute.second);
    if(error < 0)
    {
      return error;
    }
  }
  return error;
}

/**
 * @brief Returns the total number of elements in the supplied dataset
 * @param locationID The parent location that contains the dataset to read
 * @param datasetName The name of the dataset to read
 * @return Number of elements in dataset
 */
inline hsize_t getNumberOfElements(hid_t locationID, const std::string& datasetName)
{
  H5SUPPORT_MUTEX_LOCK()

  hid_t datasetID;
  herr_t error = 0;
  herr_t returnError = 0;
  hid_t dataspaceID;
  hsize_t numElements = 0;
  datasetID = H5Dopen(locationID, datasetName.c_str(), H5P_DEFAULT);
  if(datasetID < 0)
  {
    std::cout << "H5Lite.cpp::getNumberOfElements(" << __LINE__ << ") Error opening Dataset at locationID (" << locationID << ") with object name (" << datasetName << ")" << std::endl;
    return -1;
  }
  if(datasetID >= 0)
  {
    dataspaceID = H5Dget_space(datasetID);
    if(dataspaceID > 0)
    {
      int32_t rank = H5Sget_simple_extent_ndims(dataspaceID);
      if(rank > 0)
      {
        std::vector<hsize_t> dims(rank, 0); // Allocate enough room for the dims
        error = H5Sget_simple_extent_dims(dataspaceID, dims.data(), nullptr);
        numElements = std::accumulate(dims.cbegin(), dims.cend(), static_cast<hsize_t>(1), std::multiplies<hsize_t>());
      }
      error = H5Sclose(dataspaceID);
      if(error < 0)
      {
        std::cout << "Error Closing Data Space" << std::endl;
        returnError = error;
      }
    }
    else
    {
      std::cout << "Error Opening SpaceID" << std::endl;
      // returnError = dataspaceID;
    }
    error = H5Dclose(datasetID);
    if(error < 0)
    {
      std::cout << "Error Closing Dataset" << std::endl;
      returnError = error;
    }
  }
  return numElements;
}

/**
 * @brief Writes an attribute to the given object. This method is designed with
 * a Template parameter that represents a primitive value. If you need to write
 * an array, please use the other over loaded method that takes a vector.
 * @param locationID The location to look for objectName
 * @param objectName The Object to write the attribute to
 * @param attributeName The  name of the attribute
 * @param data The data to be written as the attribute
 * @return Standard HDF error condition
 */
template <typename T> inline herr_t writeScalarAttribute(hid_t locationID, const std::string& objectName, const std::string& attributeName, T data)
{
  H5SUPPORT_MUTEX_LOCK()

  hid_t objectID, dataspaceID, attributeID;
  herr_t hasAttribute;
  H5O_info_t objectInfo;
  herr_t error = 0;
  herr_t returnError = 0;
  hsize_t dims = 1;
  int32_t rank = 1;
  hid_t dataType = HDFTypeForPrimitive(data);
  if(dataType == -1)
  {
    return -1;
  }
  /* Get the type of object */
  error = H5Oget_info_by_name(locationID, objectName.c_str(), &objectInfo, H5P_DEFAULT);
  if(error < 0)
  {
    std::cout << "Error getting object info at locationID (" << locationID << ") with object name (" << objectName << ")" << std::endl;
    return error;
  }
  /* Open the object */
  objectID = openId(locationID, objectName, objectInfo.type);
  if(objectID < 0)
  {
    std::cout << "Error opening Object for Attribute operations." << std::endl;
    return static_cast<herr_t>(objectID);
  }

  /* Create the data space for the attribute. */
  dataspaceID = H5Screate_simple(rank, &dims, nullptr);
  if(dataspaceID >= 0)
  {
    /* Verify if the attribute already exists */
    hasAttribute = findAttribute(objectID, attributeName);

    /* The attribute already exists, delete it */
    if(hasAttribute == 1)
    {
      error = H5Adelete(objectID, attributeName.c_str());
      if(error < 0)
      {
        std::cout << "Error Deleting Existing Attribute" << std::endl;
        returnError = error;
      }
    }

    if(error >= 0)
    {
      /* Create the attribute. */
      attributeID = H5Acreate(objectID, attributeName.c_str(), dataType, dataspaceID, H5P_DEFAULT, H5P_DEFAULT);
      if(attributeID >= 0)
      {
        /* Write the attribute data. */
        error = H5Awrite(attributeID, dataType, &data);
        if(error < 0)
        {
          std::cout << "Error Writing Attribute" << std::endl;
          returnError = error;
        }
      }
      /* Close the attribute. */
      error = H5Aclose(attributeID);
      if(error < 0)
      {
        std::cout << "Error Closing Attribute" << std::endl;
        returnError = error;
      }
    }
    /* Close the dataspace. */
    error = H5Sclose(dataspaceID);
    if(error < 0)
    {
      std::cout << "Error Closing Dataspace" << std::endl;
      returnError = error;
    }
  }
  else
  {
    returnError = static_cast<herr_t>(dataspaceID);
  }

  /* Close the object */
  error = closeId(objectID, objectInfo.type);
  if(error < 0)
  {
    std::cout << "Error Closing HDF5 Object ID" << std::endl;
    returnError = error;
  }
  return returnError;
}

/**
 * @brief Reads data from the HDF5 File into a preallocated array.
 * @param locationID The parent location that contains the dataset to read
 * @param datasetName The name of the dataset to read
 * @param data A Pointer to the PreAllocated Array of Data
 * @return Standard HDF error condition
 */
template <typename T> inline herr_t readPointerDataset(hid_t locationID, const std::string& datasetName, T* data)
{
  H5SUPPORT_MUTEX_LOCK()

  hid_t datasetID;
  herr_t error = 0;
  herr_t returnError = 0;
  hid_t dataType = 0;
  T test = 0x00;
  dataType = HDFTypeForPrimitive(test);
  if(dataType == -1)
  {
    std::cout << "dataType was not supported." << std::endl;
    return -10;
  }
  if(locationID < 0)
  {
    std::cout << "locationID was Negative: This is not allowed." << std::endl;
    return -2;
  }
  if(nullptr == data)
  {
    std::cout << "The Pointer to hold the data is nullptr. This is NOT allowed." << std::endl;
    return -3;
  }
  datasetID = H5Dopen(locationID, datasetName.c_str(), H5P_DEFAULT);
  if(datasetID < 0)
  {
    std::cout << " Error opening Dataset: " << datasetID << std::endl;
    return -1;
  }
  if(datasetID >= 0)
  {
    error = H5Dread(datasetID, dataType, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    if(error < 0)
    {
      std::cout << "Error Reading Data." << std::endl;
      returnError = error;
    }
    error = H5Dclose(datasetID);
    if(error < 0)
    {
      std::cout << "Error Closing Dataset id" << std::endl;
      returnError = error;
    }
  }
  return returnError;
}

/**
 * @brief Reads data from the HDF5 File into an std::vector<T> object. If the dataset
 * is very large this can be an expensive method to use. It is here for convenience
 * using STL with hdf5.
 * @param locationID The parent location that contains the dataset to read
 * @param datasetName The name of the dataset to read
 * @param data A std::vector<T>. Note the vector WILL be resized to fit the data.
 * The best idea is to just allocate the vector but not to size it. The method
 * will size it for you.
 * @return Standard HDF error condition
 */
template <typename T> inline herr_t readVectorDataset(hid_t locationID, const std::string& datasetName, std::vector<T>& data)
{
  H5SUPPORT_MUTEX_LOCK()

  hid_t datasetID;
  herr_t error = 0;
  herr_t returnError = 0;
  hid_t spaceId;
  hid_t dataType;
  T test = static_cast<T>(0x00);
  dataType = HDFTypeForPrimitive(test);
  if(dataType == -1)
  {
    return -1;
  }
  datasetID = H5Dopen(locationID, datasetName.c_str(), H5P_DEFAULT);
  if(datasetID < 0)
  {
    std::cout << "H5Lite.h::readVectorDataset(" << __LINE__ << ") Error opening Dataset at locationID (" << locationID << ") with object name (" << datasetName << ")" << std::endl;
    return -1;
  }
  if(datasetID >= 0)
  {
    spaceId = H5Dget_space(datasetID);
    if(spaceId > 0)
    {
      int32_t rank = H5Sget_simple_extent_ndims(spaceId);
      if(rank > 0)
      {
        std::vector<hsize_t> dims(rank, 0); // Allocate enough room for the dims
        error = H5Sget_simple_extent_dims(spaceId, dims.data(), nullptr);
        hsize_t numElements = std::accumulate(dims.cbegin(), dims.cend(), static_cast<hsize_t>(1), std::multiplies<hsize_t>());
        // std::cout << "NumElements: " << numElements << std::endl;
        // Resize the vector
        data.resize(numElements);
        error = H5Dread(datasetID, dataType, H5S_ALL, H5S_ALL, H5P_DEFAULT, data.data());
        if(error < 0)
        {
          std::cout << "Error Reading Data.'" << datasetName << "'" << std::endl;
          returnError = error;
        }
      }
      error = H5Sclose(spaceId);
      if(error < 0)
      {
        std::cout << "Error Closing Data Space" << std::endl;
        returnError = error;
      }
    }
    else
    {
      std::cout << "Error Opening SpaceID" << std::endl;
      returnError = static_cast<herr_t>(spaceId);
    }
    error = H5Dclose(datasetID);
    if(error < 0)
    {
      std::cout << "Error Closing Dataset" << std::endl;
      returnError = error;
    }
  }
  return returnError;
}

/**
 * @brief Reads a dataset that consists of a single scalar value
 * @param locationID The HDF5 file or group id
 * @param datasetName The name or path to the dataset to read
 * @param data The variable to store the data into
 * @return HDF error condition.
 */
template <typename T> inline herr_t readScalarDataset(hid_t locationID, const std::string& datasetName, T& data)
{
  H5SUPPORT_MUTEX_LOCK()

  hid_t datasetID = 0;
  herr_t error = 0;
  herr_t returnError = 0;
  hid_t spaceId = 0;

  hid_t dataType = HDFTypeForPrimitive(data);
  if(dataType == -1)
  {
    return -1;
  }
  /* Open the dataset. */
  datasetID = H5Dopen(locationID, datasetName.c_str(), H5P_DEFAULT);
  if(datasetID < 0)
  {
    std::cout << "H5Lite.h::readScalarDataset(" << __LINE__ << ") Error opening Dataset at locationID (" << locationID << ") with object name (" << datasetName << ")" << std::endl;
    return -1;
  }
  if(datasetID >= 0)
  {
    spaceId = H5Dget_space(datasetID);
    if(spaceId > 0)
    {
      error = H5Dread(datasetID, dataType, H5S_ALL, H5S_ALL, H5P_DEFAULT, &data);
      if(error < 0)
      {
        std::cout << "Error Reading Data at locationID (" << locationID << ") with object name (" << datasetName << ")" << std::endl;
        returnError = error;
      }

      error = H5Sclose(spaceId);
      if(error < 0)
      {
        std::cout << "Error Closing Data Space at locationID (" << locationID << ") with object name (" << datasetName << ")" << std::endl;
        returnError = error;
      }
    }
    else
    {
      returnError = static_cast<herr_t>(spaceId);
    }
    error = H5Dclose(datasetID);
    if(error < 0)
    {
      std::cout << "Error Closing Dataset at locationID (" << locationID << ") with object name (" << datasetName << ")" << std::endl;
      returnError = error;
    }
  }
  return returnError;
}

/**
 * @brief Reads a dataset of multiple strings into a std::vector<std::string>
 * @param locationID
 * @param datasetName
 * @param data
 * @return
 */
inline herr_t readVectorOfStringDataset(hid_t locationID, const std::string& datasetName, std::vector<std::string>& data)
{
  H5SUPPORT_MUTEX_LOCK()

  hid_t datasetID; // dataset id
  hid_t typeID;    // type id
  herr_t error = 0;
  herr_t returnError = 0;

  datasetID = H5Dopen(locationID, datasetName.c_str(), H5P_DEFAULT);
  if(datasetID < 0)
  {
    std::cout << "H5Lite.cpp::readVectorOfStringDataset(" << __LINE__ << ") Error opening Dataset at locationID (" << locationID << ") with object name (" << datasetName << ")" << std::endl;
    return -1;
  }
  /*
   * Get the datatype.
   */
  typeID = H5Dget_type(datasetID);
  if(typeID >= 0)
  {
    hsize_t dims[1] = {0};
    /*
     * Get dataspace and allocate memory for read buffer.
     */
    hid_t dataspaceID = H5Dget_space(datasetID);
    int nDims = H5Sget_simple_extent_dims(dataspaceID, dims, nullptr);
    if(nDims != 1)
    {
      CloseH5S(dataspaceID, error, returnError);
      CloseH5T(typeID, error, returnError);
      std::cout << "H5Lite.cpp::readVectorOfStringDataset(" << __LINE__ << ") Number of dims should be 1 but it was " << nDims << ". Returning early. Is your data file correct?" << std::endl;
      return -2;
    }

    std::vector<char*> rData(dims[0], nullptr);

    /*
     * Create the memory datatype.
     */
    hid_t memtype = H5Tcopy(H5T_C_S1);
    herr_t status = H5Tset_size(memtype, H5T_VARIABLE);

    /*
     * Read the data.
     */
    status = H5Dread(datasetID, memtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, rData.data());
    if(status < 0)
    {
      status = H5Dvlen_reclaim(memtype, dataspaceID, H5P_DEFAULT, rData.data());
      CloseH5S(dataspaceID, error, returnError);
      CloseH5T(typeID, error, returnError);
      CloseH5T(memtype, error, returnError);
      std::cout << "H5Lite.cpp::readVectorOfStringDataset(" << __LINE__ << ") Error reading Dataset at locationID (" << locationID << ") with object name (" << datasetName << ")" << std::endl;
      return -3;
    }
    /*
     * copy the data into the vector of strings
     */
    data.resize(dims[0]);
    for(size_t i = 0; i < dims[0]; i++)
    {
      // printf("%s[%d]: %s\n", "VlenStrings", i, rData[i].p);
      data[i] = std::string(rData[i]);
    }
    /*
     * Close and release resources.  Note that H5Dvlen_reclaim works
     * for variable-length strings as well as variable-length arrays.
     * Also note that we must still free the array of pointers stored
     * in rData, as H5Tvlen_reclaim only frees the data these point to.
     */
    status = H5Dvlen_reclaim(memtype, dataspaceID, H5P_DEFAULT, rData.data());
    CloseH5S(dataspaceID, error, returnError);
    CloseH5T(typeID, error, returnError);
    CloseH5T(memtype, error, returnError);
  }

  CloseH5D(datasetID, error, returnError, datasetName);

  return returnError;
}

/**
 * @brief Reads a string dataset into the supplied string. Any data currently in the 'data' variable
 * is cleared first before the new data is read into the string.
 * @param locationID The parent group that holds the data object to read
 * @param datasetName The name of the dataset.
 * @param data The std::string to hold the data
 * @return Standard HDF error condition
 */
inline herr_t readStringDataset(hid_t locationID, const std::string& datasetName, std::string& data)
{
  H5SUPPORT_MUTEX_LOCK()

  hid_t datasetID; // dataset id
  hid_t typeID;    // type id
  herr_t error = 0;
  herr_t returnError = 0;
  hsize_t size;
  data.clear();
  datasetID = H5Dopen(locationID, datasetName.c_str(), H5P_DEFAULT);
  if(datasetID < 0)
  {
    std::cout << "H5Lite.cpp::readStringDataset(" << __LINE__ << ") Error opening Dataset at locationID (" << locationID << ") with object name (" << datasetName << ")" << std::endl;
    return -1;
  }
  /*
   * Get the datatype.
   */
  typeID = H5Dget_type(datasetID);
  if(typeID >= 0)
  {
    htri_t isVariableString = H5Tis_variable_str(typeID); // Test if the string is variable length

    if(isVariableString == 1)
    {
      std::vector<std::string> strings;
      error = readVectorOfStringDataset(locationID, datasetName, strings); // Read the string
      if(error < 0 || (strings.size() > 1 && !strings.empty()))
      {
        std::cout << "Error Reading string dataset. There were multiple Strings and the program asked for a single string." << std::endl;
        returnError = error;
      }
      else
      {
        data.assign(strings[0]);
      }
    }
    else
    {
      size = H5Dget_storage_size(datasetID);
      std::vector<char> buffer(static_cast<size_t>(size + 1), 0x00); // Allocate and Zero and array
      error = H5Dread(datasetID, typeID, H5S_ALL, H5S_ALL, H5P_DEFAULT, buffer.data());
      if(error < 0)
      {
        std::cout << "Error Reading string dataset." << std::endl;
        returnError = error;
      }
      else
      {
        data.append(buffer.data()); // Append the string to the given string
      }
    }
  }
  CloseH5D(datasetID, error, returnError, datasetName);
  CloseH5T(typeID, error, returnError);
  return returnError;
}

/**
 * @brief reads a null terminated string dataset into the supplied buffer. The buffer
 * should be already preallocated.
 * @param locationID The parent group that holds the data object to read
 * @param datasetName The name of the dataset.
 * @param data pointer to the buffer
 * @return Standard HDF error condition
 */
inline herr_t readStringDataset(hid_t locationID, const std::string& datasetName, char* data)
{
  H5SUPPORT_MUTEX_LOCK()

  hid_t datasetID; // dataset id
  hid_t typeID;    // type id
  herr_t error = 0;
  herr_t returnError = 0;

  datasetID = H5Dopen(locationID, datasetName.c_str(), H5P_DEFAULT);
  if(datasetID < 0)
  {
    std::cout << "H5Lite.cpp::readStringDataset(" << __LINE__ << ") Error opening Dataset at locationID (" << locationID << ") with object name (" << datasetName << ")" << std::endl;
    return -1;
  }
  typeID = H5Dget_type(datasetID);
  if(typeID >= 0)
  {
    error = H5Dread(datasetID, typeID, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    if(error < 0)
    {
      std::cout << "Error Reading string dataset." << std::endl;
      returnError = error;
    }
    CloseH5T(typeID, error, returnError);
  }
  CloseH5D(datasetID, error, returnError, datasetName);
  return returnError;
}

/**
 * @brief Returns the information about an attribute.
 * You must close the typeID argument or resource leaks will occur. Use
 *  H5Tclose(typeID); after your call to this method if you do not need the id for
 *   anything.
 * @param locationID The parent location of the Dataset
 * @param objectName The name of the dataset
 * @param attributeName The name of the attribute
 * @param dims A std::vector that will hold the sizes of the dimensions
 * @param typeClass The HDF5 class type
 * @param typeSize THe HDF5 size of the data
 * @param typeID The Attribute ID - which needs to be closed after you are finished with the data
 * @return
 */
inline herr_t getAttributeInfo(hid_t locationID, const std::string& objectName, const std::string& attributeName, std::vector<hsize_t>& dims, H5T_class_t& typeClass, size_t& typeSize, hid_t& typeID)
{
  H5SUPPORT_MUTEX_LOCK()

  /* identifiers */
  hid_t objectID;
  H5O_info_t objectInfo{};
  hid_t attributeID;
  herr_t error = 0;
  herr_t returnError = 0;
  hid_t dataspaceID;
  hid_t rank = -1;

  error = H5Oget_info_by_name(locationID, objectName.c_str(), &objectInfo, H5P_DEFAULT);
  if(error < 0)
  {
    return error;
  }

  /* Open the object */
  objectID = openId(locationID, objectName, objectInfo.type);
  if(objectID >= 0)
  {
    attributeID = H5Aopen_by_name(locationID, objectName.c_str(), attributeName.c_str(), H5P_DEFAULT, H5P_DEFAULT);
    if(attributeID >= 0)
    {
      /* Get an identifier for the datatype. */
      typeID = H5Aget_type(attributeID);
      if(typeID > 0)
      {
        /* Get the class. */
        typeClass = H5Tget_class(typeID);
        /* Get the size. */
        typeSize = H5Tget_size(typeID);
        dataspaceID = H5Aget_space(attributeID);
        if(dataspaceID >= 0)
        {
          if(typeClass == H5T_STRING)
          {
            rank = 1;
            dims.resize(rank);
            dims[0] = typeSize;
          }
          else
          {
            rank = H5Sget_simple_extent_ndims(dataspaceID);
            std::vector<hsize_t> _dims(rank, 0);
            /* Get dimensions */
            error = H5Sget_simple_extent_dims(dataspaceID, _dims.data(), nullptr);
            if(error < 0)
            {
              std::cout << "Error Getting Attribute dims" << std::endl;
              returnError = error;
            }
            // Copy the dimensions into the dims vector
            dims.clear(); // Erase everything in the Vector
            dims.resize(rank);
            std::copy(_dims.cbegin(), _dims.cend(), dims.begin());
          }
          CloseH5S(dataspaceID, error, returnError);
          dataspaceID = 0;
        }
      }
      CloseH5A(attributeID, error, returnError);
      attributeID = 0;
    }
    else
    {
      returnError = -1;
    }
    error = closeId(objectID, objectInfo.type);
    if(error < 0)
    {
      std::cout << "Error Closing Object ID" << std::endl;
      returnError = error;
    }
  }
  return returnError;
}

/**
 * @brief Reads an Attribute from an HDF5 Object.
 *
 * Use this method if you already know the datatype of the attribute. If you do
 * not know this already then use another form of this method.
 *
 * @param locationID The Parent object that holds the object to which you want to read an attribute
 * @param objectName The name of the object to which the attribute is to be read
 * @param attributeName The name of the Attribute to read
 * @param data The memory to store the data
 * @return Standard HDF Error condition
 */
template <typename T> inline herr_t readVectorAttribute(hid_t locationID, const std::string& objectName, const std::string& attributeName, std::vector<T>& data)
{
  H5SUPPORT_MUTEX_LOCK()

  /* identifiers */
  hid_t objectID;
  H5O_info_t objectInfo;
  herr_t error = 0;
  herr_t returnError = 0;
  hid_t attributeID;
  hid_t typeID;
  T test = 0x00;
  hid_t dataType = HDFTypeForPrimitive(test);
  if(dataType == -1)
  {
    return -1;
  }
  // std::cout << "   Reading Vector Attribute at Path '" << objectName << "' with Key: '" << attributeName << "'" << std::endl;
  /* Get the type of object */
  error = H5Oget_info_by_name(locationID, objectName.c_str(), &objectInfo, H5P_DEFAULT);
  if(error < 0)
  {
    return error;
  }
  /* Open the object */
  objectID = openId(locationID, objectName, objectInfo.type);
  if(objectID >= 0)
  {
    attributeID = H5Aopen_by_name(locationID, objectName.c_str(), attributeName.c_str(), H5P_DEFAULT, H5P_DEFAULT);
    if(attributeID >= 0)
    {
      // Need to allocate the array size
      H5T_class_t typeClass;
      size_t typeSize;
      std::vector<hsize_t> dims;
      error = getAttributeInfo(locationID, objectName, attributeName, dims, typeClass, typeSize, typeID);
      hsize_t numElements = std::accumulate(dims.cbegin(), dims.cend(), static_cast<hsize_t>(1), std::multiplies<hsize_t>());
      // std::cout << "    Vector Attribute has " << numElements << " elements." << std::endl;
      data.resize(numElements);
      error = H5Aread(attributeID, dataType, data.data());
      if(error < 0)
      {
        std::cout << "Error Reading Attribute." << error << std::endl;
        returnError = error;
      }
      error = H5Aclose(attributeID);
      if(error < 0)
      {
        std::cout << "Error Closing Attribute" << std::endl;
        returnError = error;
      }
    }
    else
    {
      returnError = static_cast<herr_t>(attributeID);
    }
    error = closeId(objectID, objectInfo.type);
    if(error < 0)
    {
      std::cout << "Error Closing Object" << std::endl;
      returnError = error;
    }
  }
  return returnError;
}

/**
 * @brief Reads a scalar attribute value from a dataset
 * @param locationID
 * @param objectName The name of the dataset
 * @param attributeName The name of the Attribute
 * @param data The preallocated memory for the variable to be stored into
 * @return Standard HDF5 error condition
 */
template <typename T> inline herr_t readScalarAttribute(hid_t locationID, const std::string& objectName, const std::string& attributeName, T& data)
{
  H5SUPPORT_MUTEX_LOCK()

  /* identifiers */
  hid_t objectID;
  H5O_info_t objectInfo;
  herr_t error = 0;
  herr_t returnError = 0;
  hid_t attributeID;
  T test = 0x00;
  hid_t dataType = HDFTypeForPrimitive(test);
  if(dataType == -1)
  {
    return -1;
  }
  // std::cout << "Reading Scalar style Attribute at Path '" << objectName << "' with Key: '" << attributeName << "'" << std::endl;
  /* Get the type of object */
  error = H5Oget_info_by_name(locationID, objectName.c_str(), &objectInfo, H5P_DEFAULT);
  if(error < 0)
  {
    return error;
  }
  /* Open the object */
  objectID = openId(locationID, objectName, objectInfo.type);
  if(objectID >= 0)
  {
    attributeID = H5Aopen_by_name(locationID, objectName.c_str(), attributeName.c_str(), H5P_DEFAULT, H5P_DEFAULT);
    if(attributeID >= 0)
    {
      error = H5Aread(attributeID, dataType, &data);
      if(error < 0)
      {
        std::cout << "Error Reading Attribute." << std::endl;
        returnError = error;
      }
      error = H5Aclose(attributeID);
      if(error < 0)
      {
        std::cout << "Error Closing Attribute" << std::endl;
        returnError = error;
      }
    }
    else
    {
      returnError = static_cast<herr_t>(attributeID);
    }
    error = closeId(objectID, objectInfo.type);
    if(error < 0)
    {
      std::cout << "Error Closing Object" << std::endl;
      returnError = error;
    }
  }
  return returnError;
}

/**
 * @brief Reads the Attribute into a pre-allocated pointer
 * @param locationID
 * @param objectName The name of the dataset
 * @param attributeName The name of the Attribute
 * @param data The preallocated memory for the variable to be stored into
 * @return Standard HDF5 error condition
 */
template <typename T> inline herr_t readPointerAttribute(hid_t locationID, const std::string& objectName, const std::string& attributeName, T* data)
{
  H5SUPPORT_MUTEX_LOCK()

  /* identifiers */
  hid_t objectID;
  H5O_info_t objectInfo;
  herr_t error = 0;
  herr_t returnError = 0;
  hid_t attributeID;
  T test = 0x00;
  hid_t dataType = HDFTypeForPrimitive(test);
  if(dataType == -1)
  {
    return -1;
  }
  // std::cout << "   Reading Vector Attribute at Path '" << objectName << "' with Key: '" << attributeName << "'" << std::endl;
  /* Get the type of object */
  error = H5Oget_info_by_name(locationID, objectName.c_str(), &objectInfo, H5P_DEFAULT);
  if(error < 0)
  {
    return error;
  }
  /* Open the object */
  objectID = openId(locationID, objectName, objectInfo.type);
  if(objectID >= 0)
  {
    attributeID = H5Aopen_by_name(locationID, objectName.c_str(), attributeName.c_str(), H5P_DEFAULT, H5P_DEFAULT);
    if(attributeID >= 0)
    {
      error = H5Aread(attributeID, dataType, data);
      if(error < 0)
      {
        std::cout << "Error Reading Attribute." << error << std::endl;
        returnError = error;
      }
      error = H5Aclose(attributeID);
      if(error < 0)
      {
        std::cout << "Error Closing Attribute" << std::endl;
        returnError = error;
      }
    }
    else
    {
      returnError = static_cast<herr_t>(attributeID);
    }
    error = closeId(objectID, objectInfo.type);
    if(error < 0)
    {
      std::cout << "Error Closing Object" << std::endl;
      returnError = error;
    }
  }
  return returnError;
}

/**
 * @brief Reads a string attribute from an HDF object
 * @param locationID The Parent object that holds the object to which you want to read an attribute
 * @param objectName The name of the object to which the attribute is to be read
 * @param attributeName The name of the Attribute to read
 * @param data The memory to store the data
 * @return Standard HDF Error condition
 */
inline herr_t readStringAttribute(hid_t locationID, const std::string& objectName, const std::string& attributeName, std::string& data)
{
  H5SUPPORT_MUTEX_LOCK()

  /* identifiers */
  hid_t objectID;
  H5O_info_t objectInfo{};
  hid_t attributeID;
  hid_t attributeType;
  std::vector<char> attributeOutput;
  hsize_t size;
  herr_t error = 0;
  herr_t returnError = 0;
  data.clear();
  HDF_ERROR_HANDLER_OFF;

  /* Get the type of object */
  error = H5Oget_info_by_name(locationID, objectName.c_str(), &objectInfo, H5P_DEFAULT);
  if(error < 0)
  {
    return error;
  }

  /* Open the object */
  objectID = openId(locationID, objectName, objectInfo.type);
  if(objectID >= 0)
  {
    attributeID = H5Aopen_by_name(locationID, objectName.c_str(), attributeName.c_str(), H5P_DEFAULT, H5P_DEFAULT);
    hid_t attrTypeId = H5Aget_type(attributeID);
    htri_t isVariableString = H5Tis_variable_str(attrTypeId); // Test if the string is variable length
    H5Tclose(attrTypeId);
    if(isVariableString == 1)
    {
      data.clear();
      returnError = -1;
      CloseH5A(attributeID, error, returnError);
      return returnError;
    }
    if(attributeID >= 0)
    {
      size = H5Aget_storage_size(attributeID);
      attributeOutput.resize(static_cast<size_t>(size)); // Resize the vector to the proper length
      attributeType = H5Aget_type(attributeID);
      if(attributeType >= 0)
      {
        error = H5Aread(attributeID, attributeType, attributeOutput.data());
        if(error < 0)
        {
          std::cout << "Error Reading Attribute." << std::endl;
          returnError = error;
        }
        else
        {
          if(attributeOutput[size - 1] == 0) // null Terminated string
          {
            size -= 1;
          }
          data.append(attributeOutput.data(), size); // Append the data to the passed in string
        }
        CloseH5T(attributeType, error, returnError);
      }
      CloseH5A(attributeID, error, returnError);
    }
    else
    {
      // returnError = attributeID;
    }
    error = closeId(objectID, objectInfo.type);
    if(error < 0)
    {
      std::cout << "Error Closing Object ID" << std::endl;
      returnError = error;
    }
  }
  HDF_ERROR_HANDLER_ON;
  return returnError;
}

/**
 * @brief Reads a string attribute from an HDF object into a precallocated buffer
 * @param locationID The Parent object that holds the object to which you want to read an attribute
 * @param objectName The name of the object to which the attribute is to be read
 * @param attributeName The name of the Attribute to read
 * @param data The memory to store the data into
 * @return Standard HDF Error condition
 */
inline herr_t readStringAttribute(hid_t locationID, const std::string& objectName, const std::string& attributeName, char* data)
{
  H5SUPPORT_MUTEX_LOCK()

  /* identifiers */
  hid_t objectID;
  H5O_info_t objectInfo{};
  hid_t attributeID;
  hid_t attributeType;
  herr_t error = 0;
  herr_t returnError = 0;

  HDF_ERROR_HANDLER_OFF;

  /* Get the type of object */
  error = H5Oget_info_by_name(locationID, objectName.c_str(), &objectInfo, H5P_DEFAULT);
  if(error < 0)
  {
    return error;
  }

  /* Open the object */
  objectID = openId(locationID, objectName, objectInfo.type);
  if(objectID >= 0)
  {
    attributeID = H5Aopen_by_name(locationID, objectName.c_str(), attributeName.c_str(), H5P_DEFAULT, H5P_DEFAULT);
    if(attributeID >= 0)
    {
      attributeType = H5Aget_type(attributeID);
      if(attributeType >= 0)
      {
        error = H5Aread(attributeID, attributeType, data);
        if(error < 0)
        {
          std::cout << "Error Reading Attribute." << std::endl;
          returnError = error;
        }
        CloseH5T(attributeType, error, returnError);
      }
      CloseH5A(attributeID, error, returnError);
    }
    else
    {
      // returnError = attributeID;
    }
    error = closeId(objectID, objectInfo.type);
    if(error < 0)
    {
      std::cout << "Error Closing Object ID" << std::endl;
      returnError = error;
    }
  }
  HDF_ERROR_HANDLER_ON;
  return returnError;
}

/**
 * @brief Returns the number of dimensions for a given attribute
 * @param locationID The HDF5 id of the parent group/file for the objectName
 * @param objectName The name of the dataset
 * @param attributeName The name of the attribute
 * @param rank (out) Number of dimensions is store into this variable
 */
inline herr_t getAttributeNDims(hid_t locationID, const std::string& objectName, const std::string& attributeName, hid_t& rank)
{
  H5SUPPORT_MUTEX_LOCK()

  /* identifiers */
  hid_t objectID;
  H5O_info_t objectInfo{};
  hid_t attributeID;
  herr_t error = 0;
  herr_t returnError = 0;
  hid_t dataspaceID;
  rank = -1;
  /* Get the type of object */
  error = H5Oget_info_by_name(locationID, objectName.c_str(), &objectInfo, H5P_DEFAULT);
  if(error < 0)
  {
    return error;
  }
  /* Open the object */
  objectID = openId(locationID, objectName, objectInfo.type);
  if(objectID >= 0)
  {
    attributeID = H5Aopen_by_name(locationID, objectName.c_str(), attributeName.c_str(), H5P_DEFAULT, H5P_DEFAULT);
    if(attributeID >= 0)
    {
      dataspaceID = H5Aget_space(attributeID);
      if(dataspaceID >= 0)
      {
        rank = H5Sget_simple_extent_ndims(dataspaceID);
        CloseH5S(dataspaceID, error, returnError);
      }
      CloseH5A(attributeID, error, returnError);
    }
    else
    {
      returnError = static_cast<herr_t>(attributeID);
    }
    error = closeId(objectID, objectInfo.type);
    if(error < 0)
    {
      std::cout << "Error Closing Object ID" << std::endl;
      returnError = error;
    }
  }

  return returnError;
}

/**
 * @brief Returns the number of dimensions for a given dataset
 * @param locationID The HDF5 id of the parent group/file for the objectName
 * @param objectName The name of the dataset
 * @param rank (out) Number of dimensions is store into this variable
 */
inline herr_t getDatasetNDims(hid_t locationID, const std::string& datasetName, hid_t& rank)
{
  H5SUPPORT_MUTEX_LOCK()

  hid_t datasetID;
  hid_t dataspaceID;
  herr_t error = 0;
  herr_t returnError = 0;
  rank = 0;

  /* Open the dataset. */
  if((datasetID = H5Dopen(locationID, datasetName.c_str(), H5P_DEFAULT)) < 0)
  {
    return -1;
  }

  /* Get the dataspace handle */
  dataspaceID = H5Dget_space(datasetID);
  if(dataspaceID >= 0)
  {

    /* Get rank */
    rank = H5Sget_simple_extent_ndims(dataspaceID);
    if(rank < 0)
    {
      // returnError = rank;
      rank = 0;
      std::cout << "Error Getting the rank of the dataset:" << std::endl;
    }

    /* Terminate access to the dataspace */
    CloseH5S(dataspaceID, error, returnError);
  }

  /* End access to the dataset */
  error = H5Dclose(datasetID);
  if(error < 0)
  {
    returnError = error;
    rank = 0;
  }
  return returnError;
}

/**
 * @brief Returns the H5T value for a given dataset.
 *
 * Returns the type of data stored in the dataset. You MUST use H5Tclose(typeID)
 * on the returned value or resource leaks will occur.
 * @param locationID A Valid HDF5 file or group id.
 * @param datasetName Path to the dataset
 * @return
 */
inline hid_t getDatasetType(hid_t locationID, const std::string& datasetName)
{
  H5SUPPORT_MUTEX_LOCK()

  herr_t error = 0;
  herr_t returnError = 0;
  hid_t datasetID = -1;
  /* Open the dataset. */
  if((datasetID = H5Dopen(locationID, datasetName.c_str(), H5P_DEFAULT)) < 0)
  {
    return -1;
  }
  /* Get an identifier for the datatype. */
  hid_t typeID = H5Dget_type(datasetID);
  CloseH5D(datasetID, error, returnError, datasetName);
  if(returnError < 0)
  {
    return static_cast<hid_t>(returnError);
  }
  return typeID;
}

/**
 * @brief Get the information about a dataset.
 *
 * @param locationID The parent location of the Dataset
 * @param datasetName The name of the dataset
 * @param dims A std::vector that will hold the sizes of the dimensions
 * @param typeClass The HDF5 class type
 * @param typeSize THe HDF5 size of the data
 * @return Negative value is Failure. Zero or Positive is success;
 */
inline herr_t getDatasetInfo(hid_t locationID, const std::string& datasetName, std::vector<hsize_t>& dims, H5T_class_t& classType, size_t& sizeType)
{
  H5SUPPORT_MUTEX_LOCK()

  hid_t datasetID;
  hid_t typeID;
  hid_t dataspaceID;
  herr_t error = 0;
  herr_t returnError = 0;
  hid_t rank = 0;

  /* Open the dataset. */
  if((datasetID = H5Dopen(locationID, datasetName.c_str(), H5P_DEFAULT)) < 0)
  {
    return -1;
  }

  /* Get an identifier for the datatype. */
  typeID = H5Dget_type(datasetID);
  if(typeID >= 0)
  {
    /* Get the class. */
    classType = H5Tget_class(typeID);
    /* Get the size. */
    sizeType = H5Tget_size(typeID);
    /* Release the datatype. */
    error = H5Tclose(typeID);
    if(error < 0)
    {
      std::cout << "Error Closing H5Type" << std::endl;
      returnError = error;
    }
  }
  /* Get the dataspace handle */
  dataspaceID = H5Dget_space(datasetID);
  if(dataspaceID >= 0)
  {
    /* Get the Number of Dimensions */
    rank = H5Sget_simple_extent_ndims(dataspaceID);
    if(rank > 0)
    {
      std::vector<hsize_t> _dims(rank, 0);
      /* Get dimensions */
      error = H5Sget_simple_extent_dims(dataspaceID, _dims.data(), nullptr);
      if(error < 0)
      {
        std::cout << "Error Getting Simple Extents for dataset" << std::endl;
        returnError = error;
      }
      // Copy the dimensions into the dims vector
      dims.clear(); // Erase everything in the Vector
      std::copy(_dims.cbegin(), _dims.cend(), std::back_inserter(dims));
    }
    else if(classType == H5T_STRING)
    {
      dims.clear(); // Erase everything in the Vector
      dims.push_back(sizeType);
    }
    /* Terminate access to the dataspace */
    CloseH5S(dataspaceID, error, returnError);
  }

  /* End access to the dataset */
  CloseH5D(datasetID, error, returnError, datasetName);
  return returnError;
}

}; // namespace H5Lite

#if defined(H5Support_NAMESPACE)
}
#endif
