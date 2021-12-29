/* ============================================================================
 * Copyright (c) 2009-2019 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Air Force Prime Contract FA8650-15-D-5231
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <array>
#include <numeric>

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"

#include "H5SupportTestFileLocations.h"

#include "UnitTestSupport.h"

using namespace H5Support;

namespace H5SupportTestHelper
{

namespace Constants
{
constexpr size_t dim0 = 4;
constexpr size_t dim1 = 3;
constexpr size_t dim2 = 2;

constexpr size_t rank1D = 1;
constexpr size_t rank2D = 2;
constexpr size_t rank3D = 3;
} // namespace Constants

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
herr_t testWritePointer1DArrayAttribute(hid_t file_id, const std::string& dsetName, int32_t* attributesWritten = nullptr)
{
  if(attributesWritten != nullptr)
  {
    (*attributesWritten)++;
  }

  herr_t error = -1;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr<T>();
  H5SUPPORT_REQUIRE(attributeKey.empty() == false);
  attributeKey = "1DArrayAttribute<" + attributeKey + ">";
  std::array<T, Constants::dim0> data;
  for(size_t i = 0; i < data.size(); ++i)
  {
    data[i] = static_cast<T>(i);
  }
  std::array<hsize_t, 1> dims = {data.size()};
  error = H5Lite::writePointerAttribute<T>(file_id, dsetName, attributeKey, static_cast<int32_t>(dims.size()), dims.data(), data.data());
  H5SUPPORT_REQUIRE(error >= 0);
  return error;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
herr_t testWritePointer2DArrayAttribute(hid_t file_id, const std::string& dsetName, int32_t* attributesWritten = nullptr)
{
  if(attributesWritten != nullptr)
  {
    (*attributesWritten)++;
  }

  // std::cout << DEBUG_OUT(logTime) << "testWritePointer2DArrayAttribute";
  herr_t error = -1;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr<T>();
  H5SUPPORT_REQUIRE(attributeKey.empty() == false);

  attributeKey = "2DArrayAttribute<" + attributeKey + ">";

  constexpr size_t dataSize = Constants::dim0 * Constants::dim1;

  std::array<T, dataSize> data;

  for(size_t i = 0; i < Constants::dim0; ++i)
  {
    for(size_t j = 0; j < Constants::dim1; ++j)
    {
      size_t index = (Constants::dim1 * i) + j;
      data[index] = static_cast<T>(i * j);
    }
  }
  std::array<hsize_t, Constants::rank2D> dims = {Constants::dim0, Constants::dim1};

  error = H5Lite::writePointerAttribute<T>(file_id, dsetName, attributeKey, static_cast<int32_t>(dims.size()), dims.data(), data.data());
  H5SUPPORT_REQUIRE(error >= 0);
  return error;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
herr_t testWritePointer3DArrayAttribute(hid_t file_id, const std::string& dsetName, int32_t* attributesWritten = nullptr)
{
  if(attributesWritten != nullptr)
  {
    (*attributesWritten)++;
  }

  herr_t error = -1;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr<T>();
  H5SUPPORT_REQUIRE(attributeKey.empty() == false);
  attributeKey = "3DArrayAttribute<" + attributeKey + ">";

  constexpr size_t dataSize = Constants::dim0 * Constants::dim1 * Constants::dim2;

  std::array<T, dataSize> data;

  for(size_t i = 0; i < Constants::dim0; ++i)
  {
    for(size_t j = 0; j < Constants::dim1; ++j)
    {
      for(size_t k = 0; k < Constants::dim2; ++k)
      {
        size_t index = ((Constants::dim1 * i + j)) * Constants::dim2 + k;
        data[index] = static_cast<T>(i * j * k);
      }
    }
  }

  std::array<hsize_t, Constants::rank3D> dims = {Constants::dim0, Constants::dim1, Constants::dim2};
  error = H5Lite::writePointerAttribute<T>(file_id, dsetName, attributeKey, static_cast<int32_t>(dims.size()), dims.data(), data.data());
  H5SUPPORT_REQUIRE(error >= 0);
  return error;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
herr_t testWriteVectorAttribute(hid_t file_id, std::string dsetName, int32_t* attributesWritten = nullptr)
{
  if(attributesWritten != nullptr)
  {
    (*attributesWritten)++;
  }

  herr_t error = -1;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr<T>();
  H5SUPPORT_REQUIRE(attributeKey.empty() == false);
  attributeKey = "VectorAttribute<" + attributeKey + ">";

  std::vector<hsize_t> dims = {Constants::dim0};

  /* Make dataset */
  std::vector<T> data(dims[0], 0);
  for(size_t i = 0; i < dims[0]; ++i)
  {
    data[i] = static_cast<T>(i);
  }
  // std::cout << "Attribute->Write: " << objName;
  error = H5Lite::writeVectorAttribute(file_id, dsetName, attributeKey, dims, data);
  H5SUPPORT_REQUIRE(error >= 0);
  return error;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
herr_t testWriteScalarAttribute(hid_t file_id, const std::string& dsetName, int32_t* attributesWritten = nullptr)
{
  if(attributesWritten != nullptr)
  {
    (*attributesWritten)++;
  }

  T value = 0x0F;
  herr_t error = -1;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr<T>();
  H5SUPPORT_REQUIRE(attributeKey.empty() == false);
  attributeKey = "ScalarAttribute<" + attributeKey + ">";
  error = H5Lite::writeScalarAttribute(file_id, dsetName, attributeKey, value);
  H5SUPPORT_REQUIRE(error >= 0);
  return error;
}

// -----------------------------------------------------------------------------
//  Uses Raw Pointers to save data to the data file
// -----------------------------------------------------------------------------
template <typename T>
herr_t testWritePointer2DArrayDataset(hid_t file_id, int32_t* attributesWritten = nullptr)
{
  herr_t error = 1;
  // Create the Dimensions
  std::array<hsize_t, 2> dims = {Constants::dim0, Constants::dim1};

  size_t tSize = std::accumulate(dims.cbegin(), dims.cend(), static_cast<hsize_t>(1), std::multiplies<hsize_t>());
  std::vector<T> data(tSize);
  for(int32_t i = 0; i < data.size(); ++i)
  {
    data[i] = static_cast<T>(i) * static_cast<T>(5);
  }

  std::string dsetName = H5Lite::HDFTypeForPrimitiveAsStr<T>();
  dsetName = "Pointer2DArrayDataset<" + dsetName + ">";
  std::cout << "Running " << dsetName << " ... " << '\n';
  error = H5Lite::writePointerDataset(file_id, dsetName, static_cast<int32_t>(dims.size()), dims.data(), data.data());
  H5SUPPORT_REQUIRE(error >= 0);

  error = testWritePointer3DArrayAttribute<uint8_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer3DArrayAttribute<uint16_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer3DArrayAttribute<uint32_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer3DArrayAttribute<uint64_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer3DArrayAttribute<int8_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer3DArrayAttribute<int16_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer3DArrayAttribute<int32_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer3DArrayAttribute<int64_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer3DArrayAttribute<float>(file_id, dsetName, attributesWritten);
  error = testWritePointer3DArrayAttribute<double>(file_id, dsetName, attributesWritten);

  error = testWritePointer1DArrayAttribute<int8_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer1DArrayAttribute<uint8_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer1DArrayAttribute<int16_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer1DArrayAttribute<uint16_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer1DArrayAttribute<int32_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer1DArrayAttribute<uint32_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer1DArrayAttribute<int64_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer1DArrayAttribute<uint64_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer1DArrayAttribute<float>(file_id, dsetName, attributesWritten);
  error = testWritePointer1DArrayAttribute<double>(file_id, dsetName, attributesWritten);

  error = testWritePointer2DArrayAttribute<int8_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer2DArrayAttribute<uint8_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer2DArrayAttribute<int16_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer2DArrayAttribute<uint16_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer2DArrayAttribute<int32_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer2DArrayAttribute<uint32_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer2DArrayAttribute<int64_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer2DArrayAttribute<uint64_t>(file_id, dsetName, attributesWritten);
  error = testWritePointer2DArrayAttribute<float>(file_id, dsetName, attributesWritten);
  error = testWritePointer2DArrayAttribute<double>(file_id, dsetName, attributesWritten);

  error = testWriteVectorAttribute<int8_t>(file_id, dsetName, attributesWritten);
  error = testWriteVectorAttribute<uint8_t>(file_id, dsetName, attributesWritten);
  error = testWriteVectorAttribute<int16_t>(file_id, dsetName, attributesWritten);
  error = testWriteVectorAttribute<uint16_t>(file_id, dsetName, attributesWritten);
  error = testWriteVectorAttribute<int32_t>(file_id, dsetName, attributesWritten);
  error = testWriteVectorAttribute<uint32_t>(file_id, dsetName, attributesWritten);
  error = testWriteVectorAttribute<int64_t>(file_id, dsetName, attributesWritten);
  error = testWriteVectorAttribute<uint64_t>(file_id, dsetName, attributesWritten);
  error = testWriteVectorAttribute<float>(file_id, dsetName, attributesWritten);
  error = testWriteVectorAttribute<double>(file_id, dsetName, attributesWritten);

  error = testWriteScalarAttribute<int8_t>(file_id, dsetName, attributesWritten);
  error = testWriteScalarAttribute<uint8_t>(file_id, dsetName, attributesWritten);
  error = testWriteScalarAttribute<int16_t>(file_id, dsetName, attributesWritten);
  error = testWriteScalarAttribute<uint16_t>(file_id, dsetName, attributesWritten);
  error = testWriteScalarAttribute<int32_t>(file_id, dsetName, attributesWritten);
  error = testWriteScalarAttribute<uint32_t>(file_id, dsetName, attributesWritten);
  error = testWriteScalarAttribute<int64_t>(file_id, dsetName, attributesWritten);
  error = testWriteScalarAttribute<uint64_t>(file_id, dsetName, attributesWritten);
  error = testWriteScalarAttribute<float>(file_id, dsetName, attributesWritten);
  error = testWriteScalarAttribute<double>(file_id, dsetName, attributesWritten);

  std::cout << " Passed" << '\n';
  return error;
}
} // namespace H5SupportTestHelper
