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

#ifndef UTILS_SIGNAL_H
#define UTILS_SIGNAL_H

#include <map>
#include <mutex>
#include <functional>

namespace utils
{

template <typename... Args>
class Signal
{
public:
    ~Signal()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_slots.clear();
    }

    void connect(std::function<void(Args...)> func, const void* receiver)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_slots.insert(std::make_pair(receiver, func));
    }

    void disconnect(const void* receiver)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_slots.erase(receiver);
    }

    template <typename... CallArgs>
    void operator()(CallArgs&&... args)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& func : m_slots)
        {
            func.second(std::forward<CallArgs>(args)...);
        }
    }

private:
    std::map<const void*, std::function<void(Args...)>> m_slots;
    std::mutex                                          m_mutex;
};

}

#endif