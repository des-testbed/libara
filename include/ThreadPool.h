/*
 * $FU-Copyright$
 */

#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include "ARAMacros.h"
#include "ThreadTerminatedException.h"

#include <mutex>
#include <queue>
#include <atomic>
#include <thread>
#include <vector>
#include <condition_variable>

ARA_NAMESPACE_BEGIN

class ThreadPool {
    public:
	    /**
	     * The constructor of the thread pool. It initializes an array of
         * working threads
         *
         * @param numberOfThreads The number of worker threads which should be
         * created
	     */
        ThreadPool(size_t numberOfThreads = 6);

	    /**
	     * The destructor of the thread pool. It notifies the running threads
         * and waits until the threads are finished.
	     */
        virtual ~ThreadPool();

	    /**
	     * The method is the actually executing method for a scheduled job. It
         * checks if a job is available and executes the job and waits on a
         * lock after it finishes. If a new job is available it is notified by
         * the schedule method.
	     */
        void worker();

	    /**
	     * The method schedules a task. If a worker is available the task is
         * executed in the worker() method.
         *
         * @param job The job to execute for a worker
	     */
        void schedule(std::function<void()> job);

    private:
        /**
	     * @brief The flag has to be set by the destructor and signals the worker threads that 
	     * they should terminate.
	     */
        std::atomic_bool finished;
        /// A list of worker threads
        std::vector<std::unique_ptr<std::thread>> workers;
	    /// A queue which holds jobs to be exectued by the worker threads
	    std::queue<std::function<void()>> jobs;
	    /// A lock for the job list
	    std::mutex lockJobList;
	     /// The condition variable signals that a job has been inserted in the queue
        std::condition_variable notifyJob;
};

ARA_NAMESPACE_END

#endif
