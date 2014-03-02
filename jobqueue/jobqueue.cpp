/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Cinekine Media
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE. 
 * 
 * @file    cinek/framework/jobqueue.cpp
 * @author  Samir Sinha
 * @date    2/22/2014
 * @brief   A job housekeeping and dispatcher
 * @copyright Cinekine
 */

#include "jobqueue.hpp"
#include "jobscheduler.hpp"

#include <algorithm>

namespace cinekine {
 
    /**
     * @param allocator  An (optional) allocator for custom memory
     *                   management
     */
    JobQueue::JobQueue(size_t queueLimit) :
        _nextHandle(1),
        _jobs(),
        _scheduledJobs()
    {
        _jobs.reserve(queueLimit);
        _scheduledJobs.reserve(queueLimit);
    }

    bool JobQueue::empty() const
    {
        return _jobs.empty();
    }

    JobHandle JobQueue::add(std::unique_ptr<Job>&& job)
    {
        auto jobHandleObject = JobHandleObject(_nextHandle, std::move(job));
        ++_nextHandle;
        return add(std::move(jobHandleObject));
    }

    JobHandle JobQueue::add(JobHandleObject&& jho)
    {
        auto it = std::lower_bound(_jobs.begin(), _jobs.end(),
                            jho,
                            [](const JobHandleObject& h1, const JobHandleObject& h2) -> bool
                            {
                                return h1.second->priority() < h2.second->priority();
                            });
        it = _jobs.emplace(it, std::move(jho));
        return it != _jobs.end() ? it->first : kNullJobHandle;
    }

    void JobQueue::cancel(JobHandle jobHandle)
    {
        auto it = findJob(jobHandle);
        if (it != _jobs.end())
        {
            _jobs.erase(it);
            return;
        }
        it = findScheduledJob(jobHandle);
        if (it != _scheduledJobs.end())
        {
            _scheduledJobs.erase(it);
            return;
        }
    }

    bool JobQueue::validJob(JobHandle jobHandle) const
    {
        if (findJob(jobHandle) != _jobs.end())
            return true;
        if (findScheduledJob(jobHandle) != _scheduledJobs.end())
            return true;

        return false;
    }

    Job* JobQueue::getJob(JobHandle jobHandle)
    {
        auto it = findJob(jobHandle);
        if (it != _jobs.end())
            return it->second.get();
        it = findScheduledJob(jobHandle);
        if (it != _scheduledJobs.end())
            return it->second.get();
        return nullptr;
    }

    auto JobQueue::findJob(JobHandle jobHandle) ->
        std::vector<JobHandleObject>::iterator
    {
        for (auto it = _jobs.begin(); it != _jobs.end(); ++it)
        {
            if (it->first == jobHandle)
            {
                return it;
            }           
        }
        return _jobs.end();
    }

    auto JobQueue::findScheduledJob(JobHandle jobHandle) ->
        std::vector<JobHandleObject>::iterator
    {
        for (auto scheduledIt = _scheduledJobs.begin();
             scheduledIt != _scheduledJobs.end();
             ++scheduledIt)
        {
            if (scheduledIt->first == jobHandle)
            {
                return scheduledIt;
            }   
        }
        return _scheduledJobs.end();
    }

    auto JobQueue::findJob(JobHandle jobHandle) const ->
        std::vector<JobHandleObject>::const_iterator
    {
        for (auto it = _jobs.cbegin(); it != _jobs.cend(); ++it)
        {
            if (it->first == jobHandle)
            {
                return it;
            }           
        }
        return _jobs.cend();
    }

    auto JobQueue::findScheduledJob(JobHandle jobHandle) const ->
        std::vector<JobHandleObject>::const_iterator
    {
        for (auto scheduledIt = _scheduledJobs.cbegin();
             scheduledIt != _scheduledJobs.cend();
             ++scheduledIt)
        {
            if (scheduledIt->first == jobHandle)
            {
                return scheduledIt;
            }   
        }
        return _scheduledJobs.cend();
    }

    void JobQueue::schedule()
    {
        //  moves posted jobs over to the scheduled bucket for dispatch
        if (_scheduledJobs.empty())
        {
            _scheduledJobs = std::move(_jobs);     
        }
        else
        {
            std::move(std::begin(_jobs), std::end(_jobs),
                      std::back_inserter(_scheduledJobs));
            _jobs.clear();
            std::sort(std::begin(_scheduledJobs), std::end(_scheduledJobs),
                      [](const JobHandleObject& p1, const JobHandleObject& p2) -> bool
                      {
                        return p1.second->priority() < p2.second->priority();
                      });
        }
    }

    bool JobQueue::dispatch(void* context)
    {
        if (_scheduledJobs.empty())
            return false;

        JobHandleObject& jho = _scheduledJobs.back();
        JobScheduler scheduler(*this);
        Job::Result result = jho.second->execute(scheduler, context);
        if (result == Job::kReschedule)
        {
            add(std::move(jho));
        }
        _scheduledJobs.pop_back();

        return true;
    }

} /* namespace cinekine */

