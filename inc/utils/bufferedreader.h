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

#ifndef UTILS_BUFFERED_READER_H
#define UTILS_BUFFERED_READER_H

#include <string>
#include <vector>
#include <memory>

#include "utils/readerinterface.h"

namespace utils
{

// Buffered reader should be used when lots of small io reads are done

class BufferedReader : public IReader
{
public:
    BufferedReader(std::unique_ptr<IReader> reader, size_t bufferSize);

    virtual void open(const std::string& filename);

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
    void updateOffsetsAfterSeek(uint64_t newPosition);

    std::unique_ptr<IReader>        m_Reader;
    std::vector<uint8_t>            m_Buffer;
    uint64_t                        m_BufferStartPosition;
    uint64_t                        m_BufferOffset;
    uint64_t                        m_CurrentPosition;
    uint64_t                        m_ContentLength;
    bool                            m_BufferFilled;
};

}

#endif
