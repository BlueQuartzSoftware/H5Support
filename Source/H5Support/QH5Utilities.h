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

#include <hdf5.h>

#include "H5Support/H5Support.h"
#include "H5Support/H5Utilities.h"

#ifndef H5Support_USE_QT
#error "THIS FILE SHOULD NOT BE INCLUDED UNLESS THE H5Support_USE_QT is also defined"
#endif

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QVector>

namespace H5Support
{

/**
 * @brief General Utilities for working with the HDF5 data files and API
 */
namespace QH5Utilities
{
/**
 * @brief Opens a H5 file at path filename. Can be made read only access. Returns the id of the file object.
 * @param filename
 * @param readOnly
 * @return
 */
inline hid_t openFile(const QString& filename, bool readOnly = false)
{
  return H5Utilities::openFile(filename.toStdString(), readOnly);
}

/**
 * @brief Creates a H5 file at path filename. Returns the id of the file object.
 * @param filename
 * @return
 */
inline hid_t createFile(const QString& filename)
{
  return H5Utilities::createFile(filename.toStdString());
}

/**
 * @brief Closes a H5 file object. Returns the H5 error code.
 * @param fileID
 * @return
 */
inline herr_t closeFile(hid_t& fileId)
{
  return H5Utilities::closeFile(fileId);
}

// -------------- HDF Indentifier Methods ----------------------------
/**
 * @brief Returns the path to an object
 * @param objectID The HDF5 id of the object
 * @param trim set to False to trim the path
 * @return  The path to the object relative to the objectID
 */
inline QString getObjectPath(hid_t locationID, bool trim = false)
{
  return QString::fromStdString(H5Utilities::getObjectPath(locationID, trim));
}

/**
 * @brief Returns the hdf object type
 * @param objectID The hdf5 object id
 * @param objectName The path to the data set
 * @param objectType The type of the object
 * @return  Negative value on error
 */
inline herr_t getObjectType(hid_t objectID, const QString& objectName, int32_t& objectType)
{
  return H5Utilities::getObjectType(objectID, objectName.toStdString(), objectType);
}

/**
 * @brief Retrieves the object name for a given index
 * @param fileId The hdf5 object id
 * @param index The index to retrieve the name for
 * @param name The variable to store the name
 * @return Negative value is error
 */
inline herr_t objectNameAtIndex(hid_t fileId, int32_t index, QString& name)
{
  std::string sName;
  herr_t error = H5Utilities::objectNameAtIndex(fileId, index, sName);
  name = QString::fromStdString(sName);
  return error;
}

/**
 * @brief Returns the path to an object's parent
 * @param objectID The HDF5 id of the object
 * @param trim set to False to trim the path
 * @return  The path to the object relative to the objectID
 */
inline QString getParentPath(hid_t objectID)
{
  return QString::fromStdString(H5Utilities::getParentPath(objectID));
}

/**
 * @brief Returns the path to an object's parent
 * @param objectPath The HDF5 path to the object
 * @param trim set to False to trim the path
 * @return  The path to the object relative to the objectID
 */
inline QString getParentPath(const QString& objectPath)
{
  return QString::fromStdString(H5Utilities::getParentPath(objectPath.toStdString()));
}

/**
 * @brief Returns the object name from the object's path
 * @param objectPath The HDF5 path to the object
 * @return  The object name
 */
inline QString getObjectNameFromPath(const QString& objectPath)
{
  return QString::fromStdString(H5Utilities::getObjectNameFromPath(objectPath.toStdString()));
}

/**
 * @brief Returns if a given hdf5 object is a group
 * @param objectID The hdf5 object that contains an object with name objectName
 * @param objectName The name of the object to check
 * @return True if the given hdf5 object id is a group
 */
inline bool isGroup(hid_t nodeID, const QString& objectName)
{
  return H5Utilities::isGroup(nodeID, objectName.toStdString());
}

/**
 * @brief Opens an HDF5 object for hdf5 operations
 * @param locId the Object id of the parent
 * @param objectPath The path of the object to open
 * @return The hdf5 id of the opened object. Negative value is error.
 */
inline hid_t openHDF5Object(hid_t locationID, const QString& objectName)
{
  return H5Utilities::openHDF5Object(locationID, objectName.toStdString());
}

/**
 * @brief Closes the object id
 * @param locId The object id to close
 * @return Negative value is error.
 */
inline herr_t closeHDF5Object(hid_t objectID)
{
  return H5Utilities::closeHDF5Object(objectID);
}

/**
 * @brief Returns the associated string for the given HDF class type.
 * @param classType
 * @return
 */
inline QString HDFClassTypeAsStr(hid_t classType)
{
  return QString::fromStdString(H5Utilities::HDFClassTypeAsStr(classType));
}

/**
 * @brief prints the class type of the given class
 * @param classT The Class Type to print
 */
inline void printHDFClassType(H5T_class_t classType)
{
  std::string hType = H5Utilities::HDFClassTypeAsStr(classType);
  qDebug() << QString::fromStdString(hType);
}

// -------------- HDF Group Methods ----------------------------
/**
 * @brief Returns a list of child hdf5 objects for a given object id
 * @param locationID The parent hdf5 id
 * @param typeFilter A filter to apply to the list
 * @param names Variable to store the list
 * @return
 */
inline herr_t getGroupObjects(hid_t locationID, H5Utilities::CustomHDFDataTypes typeFilter, QList<QString>& names)
{
  std::list<std::string> childNames;
  herr_t error = H5Utilities::getGroupObjects(locationID, typeFilter, childNames);

  names.clear();
  for(const auto& childName : childNames)
  {
    names.push_back(QString::fromStdString(childName));
  }

  return error;
}

/**
 * @brief Creates a HDF Group by checking if the group already exists. If the
 * group already exists then that group is returned otherwise a new group is
 * created.
 * @param locationID The HDF unique id given to files or groups
 * @param group The name of the group to create. Note that this group name should
 * not be any sort of 'path'. It should be a single group.
 */
inline hid_t createGroup(hid_t locationID, const QString& group)
{
  return H5Utilities::createGroup(locationID, group.toStdString());
}

/**
 * @brief Given a path relative to the Parent ID, this method will create all
 * the intermediate groups if necessary.
 * @param pathToCheck The path to either create or ensure exists.
 * @param parent The HDF unique id for the parent
 * @return Error Condition: Negative is error. Positive is success.
 */
inline herr_t createGroupsFromPath(const QString& pathToCheck, hid_t parent)
{
  return H5Utilities::createGroupsFromPath(pathToCheck.toStdString(), parent);
}

/**
 * @brief Given a path relative to the Parent ID, this method will create all
 * the intermediate groups if necessary.
 * @param datasetPath The path to the dataset that you want to make all the intermediate groups for
 * @param parent The HDF unique id for the parent
 * @return Error Condition: Negative is error. Positive is success.
 */
inline herr_t createGroupsForDataset(const QString& datasetPath, hid_t parent)
{
  return H5Utilities::createGroupsForDataset(datasetPath.toStdString(), parent);
}

/**
 * @brief Extracts the object name from a given path
 * @param path The path which to extract the object name
 * @return The name of the object
 */
inline QString extractObjectName(const QString& path)
{
  return QString::fromStdString(H5Utilities::extractObjectName(path.toStdString()));
}

// -------------- HDF Attribute Methods ----------------------------
/**
 * @brief Looks for an attribute with a given name
 * @param locationID The objects Parent id
 * @param objectName The name of the object
 * @param attributeName The attribute to look for (by name)
 * @return True if the attribute exists.
 */
inline bool probeForAttribute(hid_t locationID, const QString& objectName, const QString& attributeName)
{
  return H5Utilities::probeForAttribute(locationID, objectName.toStdString(), attributeName.toStdString());
}

/**
 * @brief Returns a list of all the attribute names
 * @param objectID The parent object
 * @param names Variable to hold the list of attribute names
 * @return Negate value is error
 */
inline herr_t getAllAttributeNames(hid_t objectID, QList<QString>& names)
{
  names.clear();
  std::list<std::string> attributeNames;
  herr_t err = H5Utilities::getAllAttributeNames(objectID, attributeNames);
  for(const auto& attributeName : attributeNames)
  {
    names.push_back(QString::fromStdString(attributeName));
  }
  return err;
}

/**
 * @brief Returns a list of all the attribute names
 * @param objectID The parent object
 * @param objectName The name of the object whose attribute names you want a list
 * @param names Variable to hold the list of attribute names
 * @return Negative value is error
 */
inline herr_t getAllAttributeNames(hid_t locationID, const QString& objectName, QList<QString>& names)
{
  names.clear();
  std::list<std::string> attributeNames;
  herr_t err = H5Utilities::getAllAttributeNames(locationID, objectName.toStdString(), attributeNames);
  for(const auto& attributeName : attributeNames)
  {
    names.push_back(QString::fromStdString(attributeName));
  }
  return err;
}

inline QString fileNameFromFileId(hid_t fileId)
{
  H5SUPPORT_MUTEX_LOCK()

  // Get the name of the .dream3d file that we are writing to:
  ssize_t nameSize = H5Fget_name(fileId, nullptr, 0) + 1;
  QByteArray nameBuffer(nameSize, 0);
  nameSize = H5Fget_name(fileId, nameBuffer.data(), nameSize);

  QString hdfFileName(nameBuffer);
  QFileInfo fileInfo(hdfFileName);
  hdfFileName = fileInfo.fileName();
  return hdfFileName;
}

inline QString absoluteFilePathFromFileId(hid_t fileId)
{
  H5SUPPORT_MUTEX_LOCK()

  // Get the name of the .dream3d file that we are writing to:
  ssize_t nameSize = H5Fget_name(fileId, nullptr, 0) + 1;
  QByteArray nameBuffer(nameSize, 0);
  nameSize = H5Fget_name(fileId, nameBuffer.data(), nameSize);

  QString hdfFileName(nameBuffer);
  QFileInfo fileInfo(hdfFileName);
  return fileInfo.absoluteFilePath();
}

}; // namespace QH5Utilities

}; // namespace H5Support
