//    Copyright (C) 2014 Dirk Vanden Boer <dirk.vdb@gmail.com>
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

#include "filereader.h"

#include <stdexcept>

namespace utils
{

void FileReader::open(const std::string& filename)
{
    m_fileName = filename;
    m_file.open(filename.c_str(), std::ios::binary);

    if (!m_file.is_open())
    {
        throw std::logic_error("Failed to open file for reading: " + filename);
    }
}

void FileReader::close()
{
    m_file.close();
}

uint64_t FileReader::getContentLength()
{
    uint64_t curPos = m_file.tellg();

    m_file.seekg(0, std::ios::end);
    uint64_t length = m_file.tellg();

    m_file.seekg (curPos);

    return length;
}

uint64_t FileReader::currentPosition()
{
    return m_file.tellg();
}

void FileReader::seekAbsolute(uint64_t position)
{
    if (eof())
    {
        clearErrors();
    }

    m_file.seekg(position);
}

void FileReader::seekRelative(uint64_t offset)
{
    if (eof())
    {
        clearErrors();
    }

    m_file.seekg(offset, std::ios::cur);
}

bool FileReader::eof()
{
    return m_file.eof();
}

std::string FileReader::uri()
{
    return m_fileName;
}

uint64_t FileReader::read(uint8_t* pData, uint64_t size)
{
    m_file.read(reinterpret_cast<char*>(pData), size);
    return m_file.gcount();
}

std::vector<uint8_t> FileReader::readAllData()
{
    std::vector<uint8_t> data;
    data.resize(getContentLength());

    seekAbsolute(0);
    if (data.size() != read(data.data(), data.size()))
    {
        throw std::runtime_error("Failed to read all file data for file: " + m_fileName);
    }

    return data;
}

void FileReader::clearErrors()
{
    m_file.clear();
}

}
