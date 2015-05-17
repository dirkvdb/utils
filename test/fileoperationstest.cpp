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

#include <fstream>
#include <stdexcept>
#include <vector>
#include <algorithm>

#include "utils/fileoperations.h"
#include "gtest/gtest.h"

using namespace std;
using namespace utils::fileops;

TEST(FileOperationsTest, ReadTextFile)
{
    ofstream file("testfile.txt");
    file << "line1\nline2\nline3\n";
    file.close();

    string contents = readTextFile("testfile.txt");
    EXPECT_EQ("line1\nline2\nline3\n", contents);
    deleteFile("testfile.txt");
}

TEST(FileOperationsTest, GetFileExtension)
{
    EXPECT_EQ("txt", getFileExtension("file.txt"));
    EXPECT_EQ("longextension", getFileExtension("this.file.longextension"));
    EXPECT_EQ("", getFileExtension("file."));
    EXPECT_EQ("", getFileExtension("file"));
}

TEST(FileOperationsTest, GetFileNameWithoutExtension)
{
    EXPECT_EQ("fi.le", getFileNameWithoutExtension("/path/to/fi.le.txt"));
    EXPECT_EQ("file", getFileNameWithoutExtension("/path/to/file"));
    EXPECT_EQ("fi.le", getFileNameWithoutExtension("fi.le.txt"));
    EXPECT_EQ("file", getFileNameWithoutExtension("file.txt"));
}

TEST(FileOperationsTest, GetFileNameWithoutExtensionWindows)
{
    EXPECT_EQ("fi.le", getFileNameWithoutExtension("C:\\path\\to\\fi.le.txt"));
    EXPECT_EQ("file", getFileNameWithoutExtension("C:\\path\\to\\file"));
    EXPECT_EQ("fi.le", getFileNameWithoutExtension("fi.le.txt"));
    EXPECT_EQ("file", getFileNameWithoutExtension("file.txt"));
}

TEST(FileOperationsTest, GetFileName)
{
    EXPECT_EQ("The filename (1000)", getFileName("/some/path/The filename (1000)"));
    EXPECT_EQ("Amper & sand", getFileName("/some/path/Amper & sand"));
    EXPECT_EQ("SomeFile.txt", getFileName("SomeFile.txt"));
}

TEST(FileOperationsTest, PathExists)
{
    ofstream file("testfile.txt");
    EXPECT_TRUE(pathExists("testfile.txt"));
    EXPECT_FALSE(pathExists("somefile.txt"));
    deleteFile("testfile.txt");
}

TEST(FileOperationsTest, DeleteFile)
{
    ofstream file("testfile.txt");
    EXPECT_TRUE(pathExists("testfile.txt"));
    deleteFile("testfile.txt");
    EXPECT_FALSE(pathExists("testfile.txt"));
}

TEST(FileOperationsTest, DeleteNonExistingFile)
{
    EXPECT_THROW(deleteFile("somefile.txt"), logic_error);
}

TEST(FileOperationsTest, DeleteFileOnDir)
{
    createDirectory("temp");
    EXPECT_TRUE(pathExists("temp"));
    EXPECT_THROW(deleteFile("temp"), logic_error);
    EXPECT_TRUE(pathExists("temp"));
    deleteDirectory("temp");
}

TEST(FileOperationsTest, GetPathFromFilePath)
{
    EXPECT_EQ("/", getPathFromFilepath("/temp"));
    EXPECT_EQ("/temp", getPathFromFilepath("/temp/file.txt"));
    EXPECT_EQ("/virtualDir", getPathFromFilepath("/virtualDir/testfile.txt"));
    EXPECT_EQ("temp", getPathFromFilepath("temp/aFile"));

    EXPECT_THROW(getPathFromFilepath("/temp/"), logic_error);
}

TEST(FileOperationsTest, CombinePath)
{
    EXPECT_EQ("/temp/dir", combinePath("/temp", "dir"));
    EXPECT_EQ("/temp/dir", combinePath("/temp/", "dir"));
    EXPECT_EQ("./dir", combinePath(".", "dir"));
    EXPECT_THROW(combinePath("", "dir"), logic_error);
}

TEST(FileOperationsTest, CreateDirectory)
{
    EXPECT_FALSE(pathExists("temp"));
    createDirectory("temp");
    EXPECT_TRUE(pathExists("temp"));
    deleteDirectory("temp");
}

TEST(FileOperationsTest, CreateDirectoryThatExists)
{
    createDirectory("temp");
    EXPECT_TRUE(pathExists("temp"));
    EXPECT_THROW(createDirectory("temp"), logic_error);
    deleteDirectory("temp");
}

TEST(FileOperationsTest, DeleteDirectory)
{
    createDirectory("temp");
    createDirectory("temp/temp");
    ofstream file("temp/afile.txt");
    file.close();
    EXPECT_TRUE(pathExists("temp"));
    EXPECT_TRUE(pathExists("temp/temp"));
    deleteDirectoryRecursive("temp");
    EXPECT_FALSE(pathExists("temp"));
}

TEST(FileOperationsTest, IterateDirectory)
{
    createDirectory("temp");
    createDirectory("temp/temp");
    ofstream file1("temp/afile1.txt");
    file1.close();
    ofstream file2("temp/afile2.txt");
    file2.close();
    ofstream file3("temp/temp/afile.txt");
    file3.close();


    std::vector<std::string> files;
    for (auto& entry : Directory("temp"))
    {
		files.push_back(entry.path());
	}

    deleteDirectoryRecursive("temp");

    ASSERT_EQ(3u, files.size());
    sort(files.begin(), files.end());
    EXPECT_EQ("temp/afile1.txt", files[0]);
    EXPECT_EQ("temp/afile2.txt", files[1]);
    EXPECT_EQ("temp/temp", files[2]);
}

TEST(FileOperationsTest, MoveConstuctDirectory)
{
    createDirectory("temp");

    auto dir = Directory("temp");
    Directory dir2(std::move(dir));
    EXPECT_STREQ("temp", dir2.path().c_str());

    deleteDirectoryRecursive("temp");
}

TEST(FileOperationsTest, IterateInvalidDirectory)
{
    EXPECT_THROW(Directory("doesnotexist"), std::runtime_error);
}

TEST(FileOperationsTest, GetFileSize)
{
    ofstream file("test.file");
    file << "tata";
    file.close();

    int64_t size = getFileSize("test.file");
    EXPECT_EQ(4, size);
    deleteFile("test.file");
}

TEST(FileOperationsTest, GetFileSizeBadFile)
{
    EXPECT_THROW(getFileSize("noexist"), logic_error);
}

TEST(FileOperationsTest, GetFileInfo)
{
    ofstream file("test.file");
    file << "tatata";
    file.close();

    auto info = getFileInfo("test.file");
    EXPECT_EQ(6u, info.sizeInBytes);
    deleteFile("test.file");
}

TEST(FileOperationsTest, GetFileInfoBadFile)
{
    EXPECT_THROW(getFileInfo("noexist"), logic_error);
}
