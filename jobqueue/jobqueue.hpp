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
 * @file    cinek/framework/jobqueue.hpp
 * @author  Samir Sinha
 * @date    2/17/2014
 * @brief   A job housekeeping and dispatcher
 * @copyright Cinekine
 */

#ifndef CK_FRAMEWORK_JOBQUEUE_HPP
#define CK_FRAMEWORK_JOBQUEUE_HPP

#include "job.hpp"

#include <vector>
#include <memory>

namespace cinekine {

    /**
     * @class JobQueue
     * @brief Manages a list of prioritized Jobs
     */
    class JobQueue
    {
    public:
        /**
         * @param queueLimit Number of queue entries to reserve on
         *                   initialization
         * @param context    A context pointer passed along to Jobs managed by
         *                   this queue (see Job::execute)
         */
        JobQueue(size_t queueLimit);
        /**
         * Schedules a Job object for execution based on priority.  The queue
         * dispatches the job as soon as it can, against other jobs
         * @param  job      Job pointer
         * @return          Handle to the scheduled job
         */
        JobHandle add(std::unique_ptr<Job>&& job);
        /**
         * Cancels a scheduled job.  Note this does not affect currently
         * running jobs, only queued jobs
         * @param jobHandle Handle to a scheduled job.
         */
        void cancel(JobHandle jobHandle);
        /** 
         * @param  jobHandle  Points to a job
         * @return True if the handle points to an active job
         */
        bool validJob(JobHandle jobHandle) const;
        /**
         * Returns a valid Job object
         * @param  jobHandle Handle to a job
         * @return Job pointer of nullptr if not found
         */
        Job* getJob(JobHandle jobHandle);
        /**
         * Schedules jobs for execution via a dispatcher
         */
        void schedule();
        /**
         * Executes the next job on the queue
         * @param  context  A user context pointer passed to a Job's execute
         *                  method
         * @return False if there are no more scheduled jobs
         */
        bool dispatch(void* context);
        /**
         * @return True if there are no remaining jobs on the queue
         */
        bool empty() const;

    private:
        JobHandle _nextHandle;
        typedef std::pair<JobHandle, std::unique_ptr<Job>> JobHandleObject; 
        std::vector<JobHandleObject> _jobs;
        std::vector<JobHandleObject> _scheduledJobs;

    private:
        JobHandle add(JobHandleObject&& jho);
        auto findScheduledJob(JobHandle handle) ->
            std::vector<JobHandleObject>::iterator;
        auto findJob(JobHandle handle) ->
            std::vector<JobHandleObject>::iterator;
        auto findScheduledJob(JobHandle handle) const ->
            std::vector<JobHandleObject>::const_iterator;
        auto findJob(JobHandle handle) const ->
            std::vector<JobHandleObject>::const_iterator;
    };

} /* namespace cinekine */


#endif
