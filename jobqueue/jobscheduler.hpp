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
 * @file    cinek/framework/jobscheduler.hpp
 * @author  Samir Sinha
 * @date    2/17/2014
 * @brief   A "schedule-only" interface to a JobQueue provided for Jobs
 * @copyright Cinekine
 */

#ifndef CK_FRAMEWORK_JOBSCHEDULER_HPP
#define CK_FRAMEWORK_JOBSCHEDULER_HPP

#include "job.hpp"
#include <memory>
 
namespace cinekine {
    class JobQueue;
}

namespace cinekine {
    class JobScheduler
    {
    public:
        /**
         * Constructor
         * @param queue  The owning JobQueue
         */
        JobScheduler(JobQueue& queue);
        /**
         * Schedules a Job object for execution based on priority.  The queue
         * dispatches the job as soon as it can, against other jobs
         * @param  job      Job pointer
         * @return          Handle to the scheduled job
         */
        JobHandle add(std::unique_ptr<Job>&& job);
        /**
         * Cancels a scheduled job.  Note this does not affect currently
         * running jobs, only queued jobs.
         * @param jobHandle Handle to a scheduled job.
         */
        void cancel(JobHandle jobHandle);

    private:
        JobQueue& _queue;
    };
} /* namespace cinekine */


#endif
