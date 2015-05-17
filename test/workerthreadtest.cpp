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

#include "utils/workerthread.h"
#include "gtest/gtest.h"

#include <thread>

using namespace utils;
using namespace std;
using namespace testing;

class WorkerThreadTest : public Test
{
protected:
    void SetUp()
    {
        wt.start();
    }

    void TearDown()
    {
        wt.stop();
    }

    WorkerThread wt;
};

TEST_F(WorkerThreadTest, startTwice)
{
    // first start is in teardown
    EXPECT_NO_THROW(wt.start());
}

TEST_F(WorkerThreadTest, stopTwice)
{
    // second stop is in teardown
    EXPECT_NO_THROW(wt.stop());
}

TEST_F(WorkerThreadTest, RunJobs)
{
    const long jobCount = 10;

    std::mutex mutex;
    std::condition_variable cond;

    uint32_t count = 0;
    std::set<std::thread::id> threadIds;

    for (auto i = 0; i < jobCount; ++i)
    {
        wt.addJob([&] () {
            std::lock_guard<std::mutex> lock(mutex);
            threadIds.insert(std::this_thread::get_id());
            if (++count == jobCount)
            {
                cond.notify_one();
            }
        });
    }

    std::unique_lock<std::mutex> lock(mutex);
    cond.wait(lock, [&] () { return count == jobCount; });

    EXPECT_EQ(1u, threadIds.size());
}

TEST_F(WorkerThreadTest, RunJobThatFails)
{
    std::mutex mutex;
    std::condition_variable cond;
    std::exception_ptr ex;

    wt.ErrorOccurred.connect([&] (std::exception_ptr e) {
        ex = e;
        std::lock_guard<std::mutex> lock(mutex);
        cond.notify_one();
    }, this);

    wt.addJob([&] () {
        throw std::runtime_error("Error");
    });

    {
        std::unique_lock<std::mutex> lock(mutex);
        cond.wait(lock, [&] () { return ex != nullptr; });
    }

    wt.stop();

    EXPECT_THROW(std::rethrow_exception(ex), std::runtime_error);
}

