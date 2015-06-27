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
#include "utils/stringoperations.h"

#ifdef HAVE_UTILS_CONFIG
    #include "utilsconfig.h"
#endif

#include <fstream>
#include <stdexcept>
#include <cstring>
#include <cstdlib>
#include <cinttypes>
#include <sys/stat.h>

#ifndef WIN32
    #include <unistd.h>
    #include <sys/types.h>
    #include <dirent.h>
#ifdef HAVE_XDGBASEDIR
    #include <basedir.h>
#endif
#else
    #define WIN32_LEAN_AND_MEAN 1
    #include <windows.h>
    #undef max
    #undef DELETE
    #include <shlobj.h>
    #include <direct.h>
#endif

#include <string>
#include <vector>

#include "utils/format.h"

namespace utils
{
namespace fileops
{

static const std::string g_pathSeperators = "\\/";

#ifdef WIN32
static const char g_pathSeperator = '\\';
#else
static const char g_pathSeperator = '/';
#endif

#ifndef WIN32
class Directory::DirectoryHandle
{
public:
    DirectoryHandle(const std::string& path)
    : m_pDir(opendir(path.c_str()))
    {
        if (m_pDir == nullptr)
        {
            throw std::runtime_error("Failed to open directory: " + path);
        }
    }

    DirectoryHandle(const DirectoryHandle&) = delete;
    DirectoryHandle(DirectoryHandle&& other)
    : m_pDir(std::move(other.m_pDir))
    {
        other.m_pDir = nullptr;
    }

    ~DirectoryHandle()
    {
        closedir(m_pDir);
    }

    operator DIR*() const
    {
        return m_pDir;
    }

private:
    DIR*            m_pDir;
};

Directory::Directory(const std::string& path)
: m_path(path)
, m_dirHandle(std::make_shared<DirectoryHandle>(path))
{
}

Directory::Directory(Directory&& other)
: m_path(std::move(other.path()))
, m_dirHandle(std::move(other.m_dirHandle))
{
}

std::string Directory::path() const
{
    return m_path;
}

Directory::DirectoryHandle& Directory::handle() const
{
    assert(m_dirHandle);
    return *m_dirHandle;
}

FileSystemEntry::FileSystemEntry(const std::string& path, FileSystemEntryType type)
: m_path(path)
, m_type(type)
{
}

const std::string& FileSystemEntry::path() const
{
    return m_path;
}

FileSystemEntryType FileSystemEntry::type() const
{
    return m_type;
}

FileSystemEntry& FileSystemEntry::operator=(FileSystemEntry&& other)
{
    if (this != &other)
    {
        m_path = std::move(other.m_path);
        m_type = std::move(other.m_type);
    }

    return *this;
}

struct FileSystemIterator::IteratorData
{
    IteratorData() : dirEntry(nullptr) {}

    bool operator==(const IteratorData& other)
    {
        if (!dirEntry && !other.dirEntry)
        {
            return true;
        }

        if (dirEntry || other.dirEntry)
        {
            return false;
        }

        return dirEntry->d_ino == other.dirEntry->d_ino;
    }

    dirent*             dirEntry;
    FileSystemEntry     fsEntry;
};

FileSystemIterator::FileSystemIterator()
: m_pDir(nullptr)
{
}

FileSystemIterator::FileSystemIterator(const Directory& dir)
: m_pDir(&dir)
, m_iterData(std::make_shared<IteratorData>())
{
    nextFile();
}

void FileSystemIterator::nextFile()
{
    if (!m_iterData)
    {
        throw std::logic_error("filesystem iterator out of bounds");
    }

    m_iterData->dirEntry = readdir(m_pDir->handle());
    if (!m_iterData->dirEntry)
    {
        m_iterData.reset();
        return;
    }

    if (!strcmp(m_iterData->dirEntry->d_name, ".") || !strcmp(m_iterData->dirEntry->d_name, ".."))
    {
        nextFile();
        return;
    }

    auto path = combinePath(m_pDir->path(), m_iterData->dirEntry->d_name);
    if (m_iterData->dirEntry->d_type == DT_REG)
    {
        m_iterData->fsEntry = FileSystemEntry(path, FileSystemEntryType::File);
        return;
    }

    if (m_iterData->dirEntry->d_type == DT_LNK)
    {
        m_iterData->fsEntry = FileSystemEntry(path, FileSystemEntryType::SymbolicLink);
        return;
    }

    if (m_iterData->dirEntry->d_type == DT_DIR)
    {
        m_iterData->fsEntry = FileSystemEntry(path, FileSystemEntryType::Directory);
        return;
    }

    if (m_iterData->dirEntry->d_type == DT_UNKNOWN)
    {
        //some filesystem don't support d_type use stat to get type of entry
        struct stat statInfo;
        if (stat(path.c_str(), &statInfo) == 0)
        {
            if (S_ISREG(statInfo.st_mode))
            {
                m_iterData->fsEntry = FileSystemEntry(path, FileSystemEntryType::File);
                return;
            }

            if (S_ISLNK(statInfo.st_mode))
            {
                m_iterData->fsEntry = FileSystemEntry(path, FileSystemEntryType::SymbolicLink);
                return;
            }

            if (S_ISDIR(statInfo.st_mode))
            {
                m_iterData->fsEntry = FileSystemEntry(path, FileSystemEntryType::Directory);
                return;
            }
        }
    }

    // not a file or directory, skip it
    nextFile();
}

const FileSystemEntry& FileSystemIterator::operator*()
{
    return m_iterData->fsEntry;
}

const FileSystemEntry* FileSystemIterator::operator->()
{
    return &m_iterData->fsEntry;
}

FileSystemIterator& FileSystemIterator::operator++()
{
    nextFile();
    return *this;
}

FileSystemIterator& FileSystemIterator::operator =(FileSystemIterator&& other)
{
    if (this != &other)
    {
        std::swap(m_pDir, other.m_pDir);
        m_iterData = std::move(other.m_iterData);
    }

    return *this;
}

bool FileSystemIterator::operator ==(const FileSystemIterator& other) const
{
    if (!m_iterData && !other.m_iterData)
    {
        return true;
    }

    if (!m_iterData || !other.m_iterData)
    {
        return false;
    }

    return m_iterData->fsEntry.path() == other.m_iterData->fsEntry.path();
}

bool FileSystemIterator::operator !=(const FileSystemIterator& other) const
{
    return !(*this == other);
}
#endif

std::string readTextFile(const std::string& filename)
{
    std::ifstream fileStream(filename.c_str(), std::ifstream::binary);

    if (!fileStream.is_open())
    {
        throw std::runtime_error("Failed to open file for reading: " + filename);
    }

    fileStream.seekg(0, std::ios::end);
    uint32_t length = static_cast<uint32_t>(fileStream.tellg());
    fileStream.seekg (0, std::ios::beg);

    std::vector<char> buffer(length + 1);
    fileStream.read(buffer.data(), length);
    buffer[length] = 0;

    return buffer.data();
}

std::vector<uint8_t> readFile(const std::string& filename)
{
    std::ifstream fileStream(filename.c_str(), std::ifstream::binary);

    if (!fileStream.is_open())
    {
        throw std::runtime_error("Failed to open file for reading: " + filename);
    }

    std::vector<uint8_t> data;

    fileStream.seekg(0, std::ios::end);
    uint32_t length = static_cast<uint32_t>(fileStream.tellg());
    fileStream.seekg (0, std::ios::beg);

    data.resize(length);
    fileStream.read(reinterpret_cast<char*>(data.data()), length);

    return data;
}

void writeFile(const std::vector<uint8_t>& contents, const std::string& filename)
{
    std::ofstream fileStream(filename.c_str(), std::ifstream::binary);

    if (!fileStream.is_open())
    {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }

    fileStream.write(reinterpret_cast<const char*>(contents.data()), contents.size());
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

std::string getFileName(const std::string& filepath)
{
    auto pos = filepath.find_last_of(g_pathSeperators);
    if (pos != std::string::npos && pos != filepath.size())
    {
        return filepath.substr(pos + 1, filepath.size());
    }

    return filepath;
}

std::string getFileNameWithoutExtension(const std::string& filepath)
{
    auto filename = getFileName(filepath);
    auto pos = filename.find_last_of('.');
    if (pos != std::string::npos && pos != filename.size())
    {
        return filename.substr(0, pos);
    }

    return filename;
}

uint64_t getFileSize(const std::string& filepath)
{
#ifndef WIN32
    struct stat statInfo;
    if (stat(filepath.c_str(), &statInfo) == 0)
#else
    struct __stat64 statInfo;
    if (_stat64(filepath.c_str(), &statInfo) == 0)
#endif
    {
        return statInfo.st_size;
    }

    throw std::logic_error("Failed to obtain size for file: " + filepath);
}

#ifdef WIN32
#define S_ISREG(mode) (mode & _S_IFREG)
#define S_ISLNK(mode) (false)
#define S_ISDIR(mode) (mode & _S_IFDIR)
#endif

FileSystemEntryInfo getFileInfo(const std::string& filepath)
{
#ifndef WIN32
    struct stat statInfo;
    if (stat(filepath.c_str(), &statInfo) == 0)
#else
    struct __stat64 statInfo;
    if (_stat64(filepath.c_str(), &statInfo) == 0)
#endif
    {
        FileSystemEntryInfo info;
        info.sizeInBytes    = statInfo.st_size;
        info.createTime     = statInfo.st_ctime;
        info.modifyTime     = statInfo.st_mtime;
        info.accessTime     = statInfo.st_atime;

        if (S_ISREG(statInfo.st_mode))
        {
            info.type = FileSystemEntryType::File;
        }
        else if (S_ISLNK(statInfo.st_mode))
        {
            info.type = FileSystemEntryType::SymbolicLink;
        }
        else if (S_ISDIR(statInfo.st_mode))
        {
            info.type = FileSystemEntryType::Directory;
        }
        else
        {
            info.type = FileSystemEntryType::Unknown;
        }

        return info;
    }
    else
    {
        throw std::logic_error(fmt::format("Failed to obtain file info for file: {} ({})", filepath, strerror(errno)));
    }
}

bool isRelativePath(const std::string& filepath)
{
#ifdef WIN32
    if (filepath.size() >= 2 && filepath[1] == ':')
    {
        return false;
    }
#endif

    return filepath.find_first_of(g_pathSeperators) != 0;
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
        throw std::logic_error(fmt::format("Failed to remove file: {} ({})", filepath, strerror(errno)));
    }
}

std::string getPathFromFilepath(const std::string& filepath)
{
    if (!filepath.empty() && (filepath[filepath.length() - 1] == '/' || filepath[filepath.length() - 1] == '\\'))
    {
        throw std::logic_error("Path is not a filename: " + filepath);
    }

    auto pos = filepath.find_last_of(g_pathSeperators);
    if (pos == std::string::npos)
    {
        return "";
    }
    else if (pos == 0)
    {
        return std::string(1, filepath[0]);
    }

    return filepath.substr(0, pos);
}

std::string combinePath(const std::string& left, const std::string& right)
{
    if (left.empty())
    {
        throw std::logic_error("Left part of combination is empty");
    }

    std::string path = left;
    if (left.find_last_of(g_pathSeperators) != (left.length() - 1))
    {
        path += g_pathSeperator;
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
        throw std::logic_error(fmt::format("Failed to create directory: {} ({})", path, strerror(errno)));
    }
}

void createDirectoryIfNotExists(const std::string& path)
{
    if (pathExists(path))
    {
        return;
    }

#ifndef WIN32
    if (mkdir(path.c_str(), 0755) != 0)
#else
    if (CreateDirectory(path.c_str(), nullptr) == 0)
#endif
    {
        throw std::logic_error(fmt::format("Failed to create directory: {} ({})", path, strerror(errno)));
    }
}

void deleteDirectory(const std::string& path)
{
    if (remove(path.c_str()))
    {
        throw std::logic_error("Failed to delete directory: " + path);
    }
}

#ifndef WIN32
void deleteDirectoryRecursive(const std::string& path)
{
    for (auto& entry : Directory(path))
    {
        if (entry.type() == FileSystemEntryType::Directory)
        {
            deleteDirectoryRecursive(entry.path());
        }
        else
        {
            deleteFile(entry.path());
        }
    }

    deleteDirectory(path);
}
#endif

void changeDirectory(const std::string& dir)
{
#ifndef WIN32
    if (chdir(dir.c_str()))
#else
    if (_chdir(dir.c_str()))
#endif
    {
        throw std::logic_error(fmt::format("Failed to change directory to {} ({})", dir, strerror(errno)));
    }
}

#ifndef WIN32
uint64_t countFilesInDirectory(const std::string& path, IterationType iterType)
{
    uint64_t count = 0;

    for (auto& entry : Directory(path))
    {
        switch(entry.type())
        {
            case FileSystemEntryType::File:
            case FileSystemEntryType::SymbolicLink:
                ++count;
                break;
            case FileSystemEntryType::Directory:
                if (iterType == IterationType::Recursive)
                {
                    count += countFilesInDirectory(entry.path(), iterType);
                }
                break;
            case FileSystemEntryType::Unknown:
                break;
        };
    }

    return count;
}

uint64_t calculateDirectorySize(const std::string& path, IterationType iterType)
{
    uint64_t count = 0;

    for (auto& entry : Directory(path))
    {
        if (entry.type() == FileSystemEntryType::File)
        {
            count += getFileSize(entry.path());
        }
        else if (entry.type() == FileSystemEntryType::Directory && iterType == IterationType::Recursive)
        {
            count += calculateDirectorySize(entry.path(), iterType);
        }
    }

    return count;
}

#endif

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
#ifdef HAVE_XDGBASEDIR
    xdgHandle handle;
    if (!xdgInitHandle(&handle))
    {
        throw std::logic_error("Failed to get config directory");
    }

    std::string dir = xdgConfigHome(&handle);
    xdgWipeHandle(&handle);
    return dir;
#else
    throw std::logic_error("Failed to get config directory");
#endif
#else
    return getDataDirectory();
#endif
}

std::string getDataDirectory()
{
#ifndef WIN32
#ifdef HAVE_XDGBASEDIR
    xdgHandle handle;
    if (!xdgInitHandle(&handle))
    {
        throw std::logic_error("Failed to get config directory");
    }

    std::string dir = xdgDataHome(&handle);
    xdgWipeHandle(&handle);
    return dir;
#else
    throw std::logic_error("Failed to get config directory");
#endif
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
