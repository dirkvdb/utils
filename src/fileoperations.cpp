//    Copyright (C) 2012 Dirk Vanden Boer <dirk.vdb@gmail.com>
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

#include "utils/fileoperations.h"

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <fstream>
#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>

#ifndef WIN32
    #include <unistd.h>
    #include <sys/types.h>
    #include <dirent.h>
    #include <basedir.h>
#else
    #define WIN32_LEAN_AND_MEAN 1
    #include <windows.h>
    #undef max 
    #undef DELETE
    #include <shlobj.h>
#endif

#include <string>
#include <vector>

#include "utils/log.h"
#include "utils/types.h"

namespace utils
{
namespace fileops
{

bool readTextFile(std::string& contents, const std::string& filename)
{
    std::ifstream fileStream(filename.c_str(), std::ifstream::binary);

    if (!fileStream.is_open())
    {
        return false;
    }

    fileStream.seekg(0, std::ios::end);
    uint32_t length = static_cast<uint32_t>(fileStream.tellg());
    fileStream.seekg (0, std::ios::beg);

    std::vector<char> buffer(length + 1);
    fileStream.read(&(buffer.front()), length);
    buffer[length] = 0;
    contents = &(buffer.front());

    return true;
}

bool readFile(std::vector<uint8_t>& contents, const std::string& filename)
{
    std::ifstream fileStream(filename.c_str(), std::ifstream::binary);

    if (!fileStream.is_open())
    {
        return false;
    }

    fileStream.seekg(0, std::ios::end);
    uint32_t length = static_cast<uint32_t>(fileStream.tellg());
    fileStream.seekg (0, std::ios::beg);

    contents.resize(length);
    fileStream.read(reinterpret_cast<char*>(&(contents.front())), length);

    return true;
}

std::string getFileExtension(const std::string& filepath)
{
    std::string extension;
    
    std::string::size_type pos = filepath.find_last_of('.');
    if (pos != std::string::npos && pos != filepath.size())
    {
        extension = filepath.substr(pos + 1, filepath.size());
    }
    
    return extension;
}

void getFileSize(const std::string& filepath, int64_t& sizeInBytes)
{
#ifndef WIN32
    struct stat statInfo;
    if (stat(filepath.c_str(), &statInfo) == 0)
#else
    struct __stat64 statInfo; 
    if (_stat64(filepath.c_str(), &statInfo) == 0)
#endif
    {
        sizeInBytes = statInfo.st_size;
    }
    else
    {
        throw std::logic_error("Failed to obtain size for file: " + filepath);
    }
}

void getFileInfo(const std::string& filepath, int64_t& sizeInBytes, uint32_t& modifiedTime)
{
#ifndef WIN32
    struct stat statInfo;
    if (stat(filepath.c_str(), &statInfo) == 0)
#else
    struct __stat64 statInfo; 
    if (_stat64(filepath.c_str(), &statInfo) == 0)
#endif
    {
        sizeInBytes = statInfo.st_size;
        modifiedTime = static_cast<uint32_t>(statInfo.st_ctime);
    }
    else
    {
        throw std::logic_error("Failed to obtain file info for file: " + filepath);
    }
}

bool pathExists(const std::string& filepath)
{
#ifndef WIN32
    return access(filepath.c_str(), F_OK) == 0;
#else
    return GetFileAttributes(filepath.c_str()) != 0xFFFFFFFF;
#endif

}

void deleteFile(const std::string& filepath)
{
#ifndef WIN32
    if (unlink(filepath.c_str()) == -1)
#else
    if (DeleteFile(filepath.c_str()) == FALSE)
#endif
    {
        throw std::logic_error("Failed to remove file: " + filepath);
    }
}

void getPathFromFilepath(const std::string& filepath, std::string& path)
{
    if (!filepath.empty() && filepath[filepath.length() - 1] == '/')
    {
        throw std::logic_error("Path is not a filename: " + filepath);
    }
    
    std::string::size_type pos = filepath.find_last_of('/');
    if (pos == std::string::npos)
    {
        path = "";
        return;
    }
    else if (pos == 0)
    {
        path = "/";
        return;
    }

    path = filepath.substr(0, pos);
}

std::string combinePath(const std::string& left, const std::string& right)
{
    if (left.empty())
    {
        throw std::logic_error("Left part of combination is empty");
    }
    
    std::string path = left;
    if (left[left.length() - 1] != '/')
    {
        path += '/';
    }
    
    path += right;

    return path;
}

void createDirectory(const std::string& path)
{
#ifndef WIN32
    if (mkdir(path.c_str(), 0755) != 0)
#else
    if (CreateDirectory(path.c_str(), nullptr) == 0)
#endif
    {
        throw std::logic_error("Failed to create directory: " + path);
    }
}

void deleteDirectory(const std::string& path)
{
#ifndef WIN32
    DIR* pDir = opendir(path.c_str());
    if (pDir == nullptr)
    {
        throw std::logic_error("Failed to delete directory: " + path);
    }
    
    std::string file;
    struct dirent* pDe = nullptr;

    while ((pDe = readdir(pDir)) != nullptr)
    {
        if (!strcmp(pDe->d_name, ".") || !strcmp(pDe->d_name, ".."))
            continue;

        file = combinePath(path, pDe->d_name);
        
        if (pDe->d_type == DT_DIR)
        {
            deleteDirectory(file);
        }
        else
        {
            remove(file.c_str());
        }
    }

    closedir(pDir);

    if (remove(path.c_str()))
    {
        throw std::logic_error("Failed to delete directory: " + path);
    }
#else
    std::string targetDir;
    combinePath(path, "*", targetDir);

    WIN32_FIND_DATA fd;
    HANDLE file = FindFirstFile(targetDir.c_str(), &fd);

    if (INVALID_HANDLE_VALUE == file)
    {
        throw std::logic_error("Failed to open directory for iteration: " + path);
    } 
   
    do
    {
        std::string filename = fd.cFileName;
        if (filename == "." || filename == "..")
        {
            continue;
        }

        std::string fullPath;
        combinePath(path, filename, fullPath);

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            deleteDirectory(fullPath);
        }
        else
        {
            DeleteFile(fullPath.c_str());
        }
    }
    while (FindNextFile(file, &fd) != 0);
 
    FindClose(file);
#endif
}

void iterateDirectory(const std::string& path, IFileIterator& iter)
{
#ifndef WIN32
    DIR* dp = opendir(path.c_str());

    if (dp == nullptr)
    {
        throw std::logic_error("Failed to open directory for iteration: " + path);
    }

    dirent* pDirEntry;
    bool keepGoing = true;    
    while (keepGoing && (pDirEntry = readdir(dp)))
    {
        if (!strcmp(pDirEntry->d_name, ".") || !strcmp(pDirEntry->d_name, ".."))
            continue;
            
        if (pDirEntry->d_type == DT_REG)
        {
            keepGoing = iter.onFile(path + "/" + pDirEntry->d_name);
        }
        else if (pDirEntry->d_type == DT_DIR)
        {
            try
            {
                iterateDirectory(path + "/" + pDirEntry->d_name, iter);
            }
            catch (std::exception& e)
            {
                log::warn(e.what());
            }
        }
        else if (pDirEntry->d_type == DT_UNKNOWN)
        {
            //some filesystem don't support d_type use stat to get type of entry
            std::string filepath = path + "/" + pDirEntry->d_name;
            struct stat statInfo;
            if (stat(filepath.c_str(), &statInfo) == 0)
            {
                if (S_ISREG(statInfo.st_mode))
                {
                    keepGoing = iter.onFile(filepath);
                }
                else if (S_ISDIR(statInfo.st_mode))
                {
                    try
                    {
                        iterateDirectory(filepath, iter);
                    }
                    catch (std::exception& e)
                    {
                        log::warn(e.what());
                    }
                }
            }
            else
            {
                log::debug("Failed to stat file:", path);
            }
        }
    }

    closedir(dp);
#else
    std::string targetDir;
    combinePath(path, "*", targetDir);

    WIN32_FIND_DATA fd;
    HANDLE file = FindFirstFile(targetDir.c_str(), &fd);

    if (INVALID_HANDLE_VALUE == file)
    {
        throw std::logic_error("Failed to open directory for iteration: " + path);
    } 
   
    bool keepGoing = true;

    do
    {
        std::string filename = fd.cFileName;
        if (filename == "." || filename == "..")
        {
            continue;
        }

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            try
            {
                std::string subdir;
                combinePath(path, filename, subdir);
                iterateDirectory(subdir, iter);
            }
            catch(exception& e)
            {
                log::warn(e.what());
            }
        }
        else
        {
            std::string filepath;
            combinePath(path, filename, filepath);
            keepGoing = iter.onFile(filepath);
        }
    }
    while (keepGoing && FindNextFile(file, &fd) != 0);
 
    FindClose(file);
#endif
}

int32_t countFilesInDirectory(const std::string& path)
{
    int32_t numFiles = 0;

#ifndef WIN32
    DIR* dp = opendir(path.c_str());

    if (dp == nullptr)
    {
        throw std::logic_error("Failed to open directory for iteration: " + path);
    }

    dirent* pDirEntry;
    while ((pDirEntry = readdir(dp)))
    {
        if (!strcmp(pDirEntry->d_name, ".") || !strcmp(pDirEntry->d_name, ".."))
            continue;
            
        if (pDirEntry->d_type == DT_REG)
        {
            ++numFiles;
        }
        else if (pDirEntry->d_type == DT_DIR)
        {
            try
            {
                numFiles += countFilesInDirectory(path + "/" + pDirEntry->d_name);
            }
            catch (std::exception& e)
            {
            }
        }
        else if (pDirEntry->d_type == DT_UNKNOWN)
        {
            //some filesystem don't support d_type use stat to get type of entry
            std::string filepath = path + "/" + pDirEntry->d_name;
            struct stat statInfo;
            if (stat(filepath.c_str(), &statInfo) == 0)
            {
                if (S_ISREG(statInfo.st_mode))
                {
                    ++numFiles;
                }
                else if (S_ISDIR(statInfo.st_mode))
                {
                    try
                    {
                        numFiles += countFilesInDirectory(path + "/" + pDirEntry->d_name);
                    }
                    catch (std::exception& e)
                    {
                    }
                }
            }
            else
            {
                log::debug("Failed to stat file:", path);
            }
        }
    }

    closedir(dp);
#else
    std::string targetDir;
    combinePath(path, "*", targetDir);

    WIN32_FIND_DATA fd;
    HANDLE file = FindFirstFile(targetDir.c_str(), &fd);

    if (INVALID_HANDLE_VALUE == file)
    {
        throw std::logic_error("Failed to open directory for iteration: " + path);
    } 
   
    do
    {
        std::string filename = fd.cFileName;
        if (filename == "." || filename == "..")
        {
            continue;
        }

        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            try
            {
                std::string subdir;
                combinePath(path, filename, subdir);
                numFiles += countFilesInDirectory(subdir);
            }
            catch(exception& e)
            {
                log::warn(e.what());
            }
        }
        else
        {
            ++numFiles;
        }
    }
    while (FindNextFile(file, &fd) != 0);
 
    FindClose(file);
#endif

    return numFiles;
}

std::string getHomeDirectory()
{
#ifndef WIN32
    const char* home = getenv("HOME");
    if (home == nullptr)
    {
        throw std::logic_error("Failed to get home environment variable");
    }

    return home;
#else
    return getDataDirectory();
#endif
}

std::string getConfigDirectory()
{
#ifndef WIN32
    xdgHandle handle;
    if (!xdgInitHandle(&handle))
    {
        throw std::logic_error("Failed to get config directory");
    }
    
    std::string dir = xdgConfigHome(&handle);
    xdgWipeHandle(&handle);
    
    return dir;
#else
    return getDataDirectory();
#endif
}

std::string getDataDirectory()
{
#ifndef WIN32
    xdgHandle handle;
    if (!xdgInitHandle(&handle))
    {
        throw std::logic_error("Failed to get config directory");
    }
    
    std::string dir = xdgDataHome(&handle);
    xdgWipeHandle(&handle);
    
    return dir;
#else
    char path[MAX_PATH];

    if (SHGetSpecialFolderPath(0, path, CSIDL_LOCAL_APPDATA, TRUE) == FALSE)
    {
        throw std::logic_error("Failed to get config directory");
    }

    return path;
#endif
}

}
}
