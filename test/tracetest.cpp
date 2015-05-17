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
#include <functional>

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
        : m_ev(PerfLogger::createTask("TestTracer"))
        , m_queuePerf(PerfLogger::createQueue("TestQueue"))
        , m_note(PerfLogger::createNote("TestDetails"))
        {
        }

        void m1()
        {
            TraceMethod();
        }

        void m2()
        {
            TraceMethod();
        }

        void interrupt()
        {
            TraceInterrupt();
        }

        void queueAdd()
        {
            m_note->addNote("QAdd");
            m_queuePerf->itemAdded();
            m_note->addNote("QAddDone");
        }

        void queueRemove()
        {
            m_queuePerf->itemRemoved();
        }

    private:
        ScopedPerfTrace m_ev;
        QueueEventPtr m_queuePerf;
        NoteEventPtr m_note;
    };

    PerfLogger::enable();

    {
        TestTracer t;
        t.m1();
        t.queueAdd();
        t.queueAdd();
        t.m2();
        t.queueRemove();
        t.interrupt();
        t.queueRemove();
        t.m2();
    }

    PerfLogger::disable();
    PerfLogger::writeToFile("trace.tdi");
}
