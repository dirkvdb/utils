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

#include "utils/trace.h"

#include <memory>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace utils;
using namespace testing;
using namespace std::placeholders;

class TraceTest : public Test
{
};

TEST_F(TraceTest, PerformTrace)
{
    class TestTracer
    {
    public:
        TestTracer()
        : m_ev(PerfLogger::createEvent(EventType::Task, "TestTracer")) {}
    
        void m1()
        {
            TraceMethod(EventType::Task);
        }
        
        void m2()
        {
            TraceMethod(EventType::Task);
        }
        
        void interrupt()
        {
            TraceMethod(EventType::Interrupt);
        }
        
    private:
        ScopedPerfTrace m_ev;
    };
    
    PerfLogger::enable();
    
    {
        TestTracer t;
        t.m1();
        t.m2();
        t.interrupt();
    }
    
    PerfLogger::disable();
    PerfLogger::writeToFile("trace.tdi");
}
