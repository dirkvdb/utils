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

#ifndef UTILS_FILE_READER_H
#define UTILS_FILE_READER_H

#include <string>
#include <fstream>

#include "utils/readerinterface.h"

namespace utils
{

class FileReader : public IReader
{
public:
    virtual void open(const std::string& filename) override;

    virtual uint64_t getContentLength() override;
    virtual uint64_t currentPosition() override;
    virtual bool eof() override;
    virtual std::string uri() override;
    
    virtual void seekAbsolute(uint64_t position) override;
    virtual void seekRelative(uint64_t offset) override;
    virtual uint64_t read(uint8_t* pData, uint64_t size) override;
    virtual std::vector<uint8_t> readAllData() override;
    virtual void clearErrors() override;

private:
    std::string         m_FileName;
    std::ifstream       m_File;
};

}

#endif
