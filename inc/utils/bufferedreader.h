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

    void open(const std::string& filename) override;
    void close() override;

    uint64_t getContentLength() override;
    uint64_t currentPosition() override;
    bool eof() override;
    std::string uri() override;

    void seekAbsolute(uint64_t position) override;
    void seekRelative(uint64_t offset) override;
    uint64_t read(uint8_t* pData, uint64_t size) override;
    std::vector<uint8_t> readAllData() override;
    void clearErrors() override;

private:
    void updateOffsetsAfterSeek(uint64_t newPosition);

    std::unique_ptr<IReader> m_reader;
    std::vector<uint8_t> m_buffer;
    uint64_t m_bufferStartPosition;
    uint64_t m_bufferOffset;
    uint64_t m_currentPosition;
    uint64_t m_contentLength;
    bool m_bufferFilled;
};
}

#endif
