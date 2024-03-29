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

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"

#include "H5Support/H5Support.h"
namespace H5Support {

/**
 * @brief The HDF5FileSentinel class ensures the HDF5 file that is currently open
 * is closed when the variable goes out of Scope
 */
class H5ScopedFileSentinel
{
public:
  H5ScopedFileSentinel(hid_t fileID, bool turnOffErrors)
  : m_FileID(fileID)
  , m_TurnOffErrors(turnOffErrors)
  {
    if(m_TurnOffErrors)
    {
      H5Eget_auto(H5E_DEFAULT, &_oldHDF_error_func, &_oldHDF_error_client_data);
      H5Eset_auto(H5E_DEFAULT, nullptr, nullptr);
    }
  }
  ~H5ScopedFileSentinel()
  {
    if(m_TurnOffErrors)
    {
      H5Eset_auto(H5E_DEFAULT, _oldHDF_error_func, _oldHDF_error_client_data);
    }
    for(auto temp : m_Groups)
    {
      if(temp > 0)
      {
        H5Gclose(temp);
      }
    }

    if(m_FileID > 0)
    {
      H5Utilities::closeFile(m_FileID);
    }
  }

  H5ScopedFileSentinel(const H5ScopedFileSentinel&) = delete;            // Copy Constructor Not Implemented
  H5ScopedFileSentinel(H5ScopedFileSentinel&&) = delete;                 // Move Constructor Not Implemented
  H5ScopedFileSentinel& operator=(const H5ScopedFileSentinel&) = delete; // Copy Assignment Not Implemented
  H5ScopedFileSentinel& operator=(H5ScopedFileSentinel&&) = delete;      // Move Assignment Not Implemented

  void setFileID(hid_t fileID)
  {
    m_FileID = fileID;
  }

  hid_t getFileID()
  {
    return m_FileID;
  }

  void addGroupId(hid_t groupID)
  {
    m_Groups.push_back(groupID);
  }

private:
  hid_t m_FileID = -1;
  bool m_TurnOffErrors = false;
  std::vector<hid_t> m_Groups;

  herr_t (*_oldHDF_error_func)(hid_t, void*){};
  void* _oldHDF_error_client_data{};
};

/**
 * @brief The H5ScopedGroupSentinel class ensures the HDF5 group that is currently open
 * is closed when the variable goes out of Scope
 */
class H5ScopedGroupSentinel
{
public:
  H5ScopedGroupSentinel(hid_t groupID, bool turnOffErrors)
  : m_TurnOffErrors(turnOffErrors)
  {
    m_Groups.push_back(groupID);
    if(m_TurnOffErrors)
    {
      H5Eget_auto(H5E_DEFAULT, &_oldHDF_error_func, &_oldHDF_error_client_data);
      H5Eset_auto(H5E_DEFAULT, nullptr, nullptr);
    }
  }
  ~H5ScopedGroupSentinel()
  {
    if(m_TurnOffErrors)
    {
      H5Eset_auto(H5E_DEFAULT, _oldHDF_error_func, _oldHDF_error_client_data);
    }
    for(auto temp : m_Groups)
    {
      if(temp > 0)
      {
        H5Gclose(temp);
      }
    }
  }

  H5ScopedGroupSentinel(const H5ScopedGroupSentinel&) = delete;            // Copy Constructor Not Implemented
  H5ScopedGroupSentinel(H5ScopedGroupSentinel&&) = delete;                 // Move Constructor Not Implemented
  H5ScopedGroupSentinel& operator=(const H5ScopedGroupSentinel&) = delete; // Copy Assignment Not Implemented
  H5ScopedGroupSentinel& operator=(H5ScopedGroupSentinel&&) = delete;      // Move Assignment Not Implemented

  void addGroupId(hid_t groupID)
  {
    m_Groups.push_back(groupID);
  }

private:
  bool m_TurnOffErrors;
  std::vector<hid_t> m_Groups;

  herr_t (*_oldHDF_error_func)(hid_t, void*){};
  void* _oldHDF_error_client_data{};
};

/**
 * @brief The H5ScopedObjectSentinel class ensures the HDF5 object that is currently open
 * is closed when the variable goes out of Scope
 */
class H5ScopedObjectSentinel
{
public:
  H5ScopedObjectSentinel(hid_t objectID, bool turnOffErrors)
  : m_TurnOffErrors(turnOffErrors)
  {
    m_Objects.push_back(objectID);
    if(m_TurnOffErrors)
    {
      H5Eget_auto(H5E_DEFAULT, &_oldHDF_error_func, &_oldHDF_error_client_data);
      H5Eset_auto(H5E_DEFAULT, nullptr, nullptr);
    }
  }

  ~H5ScopedObjectSentinel()
  {
    if(m_TurnOffErrors)
    {
      H5Eset_auto(H5E_DEFAULT, _oldHDF_error_func, _oldHDF_error_client_data);
    }
    for(auto temp : m_Objects)
    {
      if(temp > 0)
      {
        H5Utilities::closeHDF5Object(temp);
      }
    }
  }

  H5ScopedObjectSentinel(const H5ScopedObjectSentinel&) = delete;            // Copy Constructor Not Implemented
  H5ScopedObjectSentinel(H5ScopedObjectSentinel&&) = delete;                 // Move Constructor Not Implemented
  H5ScopedObjectSentinel& operator=(const H5ScopedObjectSentinel&) = delete; // Copy Assignment Not Implemented
  H5ScopedObjectSentinel& operator=(H5ScopedObjectSentinel&&) = delete;      // Move Assignment Not Implemented

  void addObjectID(hid_t objectID)
  {
    m_Objects.push_back(objectID);
  }

private:
  bool m_TurnOffErrors;
  std::vector<hid_t> m_Objects;

  herr_t (*_oldHDF_error_func)(hid_t, void*){};
  void* _oldHDF_error_client_data{};
};

/**
 * @brief The H5GroupAutoCloser class
 */
class H5GroupAutoCloser
{
public:
  H5GroupAutoCloser(hid_t groupId)
  : gid(groupId)
  {
  }

  ~H5GroupAutoCloser()
  {
    if(gid > 0)
    {
      H5Gclose(gid);
    }
  }

private:
  hid_t gid = -1;
};

}; // namespace H5Support
