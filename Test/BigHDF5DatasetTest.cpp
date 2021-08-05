/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <array>
#include <iostream>
#include <string>
#include <vector>

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"

int main(int argc, char* argv[])
{
  std::string filePath("/tmp/BIG_HDF5_DATASET.h5");

  if(argc > 1)
  {
    filePath = argv[1];
  }

  std::cout << "Test starting" << std::endl;
  std::cout << "Writing to " << filePath << '\n';
  hsize_t size = 5294967296ull;
  std::vector<unsigned char> data(size);

  hid_t fileId = H5Utilities::createFile(filePath);
  hid_t groupId = H5Utilities::createGroup(fileId, "big_data");
  if(groupId < 0)
  {
    std::cout << "Error creating Group" << std::endl;
    return EXIT_FAILURE;
  }

  std::array<hsize_t, 1> dims{size};
  herr_t error = H5Lite::writePointerDataset(groupId, "TEST", static_cast<int32_t>(dims.size()), dims.data(), data.data());
  if(error < 0)
  {
    return EXIT_FAILURE;
  }
  H5Utilities::closeHDF5Object(groupId);
  H5Utilities::closeFile(fileId);
  return EXIT_SUCCESS;
}
