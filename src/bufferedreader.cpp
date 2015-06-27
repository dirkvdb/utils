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

#include "utils/bufferedreader.h"

#include <cassert>
#include <cstring>
#include <algorithm>
#include <stdexcept>

namespace utils
{

BufferedReader::BufferedReader(std::unique_ptr<utils::IReader> reader, size_t bufferSize)
: m_reader(std::move(reader))
, m_buffer(bufferSize, '\0')
, m_bufferStartPosition(0)
, m_bufferOffset(0)
, m_currentPosition(0)
, m_contentLength(0)
, m_bufferFilled(false)
{
}

void BufferedReader::open(const std::string& filename)
{
    m_reader->open(filename);
    m_contentLength = m_reader->getContentLength();
    m_bufferStartPosition = 0;
    m_currentPosition = 0;
    m_bufferOffset = 0;
    m_bufferFilled = false;
}

void BufferedReader::close()
{
    m_reader->close();
}

uint64_t BufferedReader::getContentLength()
{
    return m_contentLength;
}

uint64_t BufferedReader::currentPosition()
{
    return m_currentPosition;
}

void BufferedReader::seekAbsolute(uint64_t position)
{
    updateOffsetsAfterSeek(position);
}

void BufferedReader::seekRelative(uint64_t offset)
{
    updateOffsetsAfterSeek(m_currentPosition + offset);
}

bool BufferedReader::eof()
{
    return m_currentPosition >= m_contentLength;
}

std::string BufferedReader::uri()
{
    return m_reader->uri();
}

uint64_t BufferedReader::read(uint8_t* pData, uint64_t size)
{
    if (!m_bufferFilled)
    {
        if (size < m_buffer.size())
        {
            m_reader->seekAbsolute(m_currentPosition);
            auto read = m_reader->read(m_buffer.data(), m_buffer.size());

            if (read > 0)
            {
                auto advance = std::min(read, size);

                memcpy(pData, m_buffer.data(), size);
                m_bufferStartPosition = m_currentPosition;
                m_bufferFilled = true;
                m_currentPosition += advance;
                m_bufferOffset = advance;
                return advance;
            }
            else
            {
                return read;
            }
        }
        else
        {
            // requested data larger then buffer, read directly
            m_reader->seekAbsolute(m_currentPosition);
            m_currentPosition = m_reader->read(pData, size);
            return size;
        }
    }
    else
    {
        auto bytesInBuffer = m_buffer.size() - m_bufferOffset;

        if (size < bytesInBuffer)
        {
            // sufficient data in the buffer
            memcpy(pData, &m_buffer[m_bufferOffset], size);
            m_currentPosition += size;
            m_bufferOffset += size;

            return size;
        }
        else
        {
            // first copy the existing data from the buffer
            memcpy(pData, &m_buffer[m_bufferOffset], bytesInBuffer);
            m_currentPosition += bytesInBuffer;

            // we are going to make a read, make sure the read pointer is ok
            m_reader->seekAbsolute(m_currentPosition);

            if (size - bytesInBuffer < m_buffer.size())
            {
                // one bufferfill is sufficient to provide the data

                // fill the buffer with fresh data
                auto read = m_reader->read(m_buffer.data(), m_buffer.size());
                auto bytesToRead = std::min(read, size - bytesInBuffer);

                if (bytesToRead > 0)
                {
                    // copy the reaming data from the buffer
                    memcpy(pData + bytesInBuffer, m_buffer.data(), bytesToRead);

                    m_bufferStartPosition = m_currentPosition;
                    m_currentPosition += bytesToRead;
                    m_bufferOffset = bytesToRead;

                    assert(m_bufferOffset <= m_buffer.size());
                    if (m_bufferOffset == m_buffer.size())
                    {
                        // buffer was completely emptied, mark it as not filled
                        m_bufferFilled = false;
                    }
                }
                else
                {
                    // buffer is not filled as we could not obtain extra data
                    m_bufferFilled = false;
                }

                return bytesInBuffer + bytesToRead;
            }
            else
            {
                // rest of the data is larger then 1 buffer so read it directly
                auto read = m_reader->read(pData + bytesInBuffer, size - bytesInBuffer);
                m_currentPosition += read;
                m_bufferFilled = false;

                return read + bytesInBuffer;
            }
        }
    }
}

std::vector<uint8_t> BufferedReader::readAllData()
{
    m_currentPosition = m_contentLength;
    return m_reader->readAllData();
}

void BufferedReader::clearErrors()
{
    m_reader->clearErrors();
}

void BufferedReader::updateOffsetsAfterSeek(uint64_t newPosition)
{
    if (m_bufferFilled && (newPosition >= m_bufferStartPosition && newPosition < (m_bufferStartPosition + m_buffer.size())))
    {
        // seek is in the buffer range, update the offset
        m_bufferOffset = newPosition - m_bufferStartPosition;
    }
    else
    {
        // seek is not in the buffer range, so buffer is not valid
        m_bufferFilled = false;
    }

    m_currentPosition = newPosition;
}

}
