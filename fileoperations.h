//    Copyright (C) 2009 Dirk Vanden Boer <dirk.vdb@gmail.com>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

#include <string>
#include <vector>

#include "types.h"

namespace FileOperations
{
    class IFileIterator
    {
    public:
        virtual ~IFileIterator() {}
        virtual bool onFile(const std::string& filename) = 0;
    };
    
    bool readTextFile(std::string& contents, const std::string& filepath);
    bool readFile(std::vector<uint8_t>& contents, const std::string& filepath);
    std::string getFileExtension(const std::string& filepath);
    void getFileSize(const std::string& filepath, int64_t& sizeInBytes);
    void getFileInfo(const std::string& filepath, int64_t& sizeInBytes, uint32_t& modifiedTime);

    bool pathExists(const std::string& path);
    void deleteFile(const std::string& filepath);

    void getPathFromFilepath(const std::string& filepath, std::string& path);
    void combinePath(const std::string& left, const std::string& right, std::string& out);
    std::string combinePath(const std::string& left, const std::string& right);

    void createDirectory(const std::string& path);
    void deleteDirectory(const std::string& path);
    void iterateDirectory(const std::string& path, IFileIterator& iter);
    int32_t countFilesInDirectory(const std::string& path);

    std::string getHomeDirectory();
    std::string getConfigDirectory();
    std::string getDataDirectory();
}

#endif
