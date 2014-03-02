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
 * @file    cinek/framework/job.hpp
 * @author  Samir Sinha
 * @date    2/17/2014
 * @brief   Common framework-level types
 * @copyright Cinekine
 */

#ifndef CK_FRAMEWORK_JOB_HPP
#define CK_FRAMEWORK_JOB_HPP

#include "jobtypes.hpp"

namespace cinekine {
    class JobScheduler;
}

namespace cinekine {
    
    /**
     * @class Job
     * @brief A standalone runner object 
     */
    class Job
    {
    public:
        virtual ~Job() = default;
        /**
         * The Job::execute() result used by callers to determine post-execute
         * behavior
         */
        enum Result
        {
            kTerminate,     /**< Instruct callers to terminate the job */
            kReschedule     /**< Instruct callers to reschedule the job */
        };
        /**
         * Called When the Job executes
         * @param  scheduler The owning JobQueue's scheduled, used by Job
         *                   objects to schedule Jobs
         * @param  context   Context pointer shared with other Jobs managed by
         *                   their common JobQueue/JobScheduler
         * @return Signals the Job's status to callers 
         */
        virtual Result execute(JobScheduler& scheduler,
                               void* context) = 0;
        /** 
         * Defines the Job's priority
         * @return A relative priority (0 = normal)
         */
        virtual int32_t priority() const = 0;
    };

} /* namespace cinekine */


#endif