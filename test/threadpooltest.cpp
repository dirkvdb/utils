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

#include "utils/threadpool.h"
#include "gtest/gtest.h"

#include <chrono>
#include <thread>

using namespace utils;
using namespace std;
using namespace testing;

static constexpr uint32_t g_poolSize = 4;

class ThreadPoolTest : public Test
{
protected:
    ThreadPoolTest()
    : tp(g_poolSize)
    {
    }

    void SetUp()
    {
        tp.start();
    }
    
    void TearDown()
    {
        tp.stop();
    }
    
    ThreadPool tp;
};

TEST_F(ThreadPoolTest, startTwice)
{
    // first start is in teardown
    EXPECT_NO_THROW(tp.start());
}

TEST_F(ThreadPoolTest, stopTwice)
{
    // second stop is in teardown
    EXPECT_NO_THROW(tp.stop());
}

TEST_F(ThreadPoolTest, RunJobs)
{
    const long jobCount = g_poolSize * 100;

    std::mutex mutex;
    std::condition_variable cond;
    std::condition_variable jobCond;
    
    uint32_t count = 0;
    std::set<std::thread::id> threadIds;

    for (auto i = 0; i < jobCount; ++i)
    {
        tp.addJob([&] () {
            std::unique_lock<std::mutex> lock(mutex);
            threadIds.insert(std::this_thread::get_id());

            if (count < g_poolSize)
            {
                jobCond.wait_for(lock, std::chrono::milliseconds(10));
            }

            if (++count == jobCount)
            {
                cond.notify_one();
            }
        });

        jobCond.notify_one();
    }
    
    std::unique_lock<std::mutex> lock(mutex);
    cond.wait(lock, [&] () { return count == jobCount; });
    
    EXPECT_EQ(g_poolSize, static_cast<uint32_t>(threadIds.size()));
}

