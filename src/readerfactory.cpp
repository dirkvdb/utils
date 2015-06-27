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

#include "utils/readerfactory.h"

#include <stdexcept>

#include "utils/filereader.h"
#include "utils/bufferedreader.h"

namespace utils
{

std::vector<std::unique_ptr<IReaderBuilder>> ReaderFactory::m_builders;

void ReaderFactory::registerBuilder(std::unique_ptr<IReaderBuilder> builder)
{
    m_builders.push_back(std::move(builder));
}

IReader* ReaderFactory::create(const std::string& uri)
{
    for (auto& builder : m_builders)
    {
        if (builder->supportsUri(uri))
        {
            return builder->build(uri);
        }
    }

    // by default just try to read it from the filesystem
    return new FileReader();
}

IReader* ReaderFactory::createBuffered(const std::string& filepath, uint32_t bufferSize)
{
    return new BufferedReader(std::unique_ptr<utils::IReader>(create(filepath)), bufferSize);
}

}
