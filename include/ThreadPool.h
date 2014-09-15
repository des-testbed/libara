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
	     *
	     */
        ThreadPool(size_t numberOfThreads = std::thread::hardware_concurrency());

	    /**
	     *
	     */
        virtual ~ThreadPool();

	    /**
	     *
	     */
        void worker();

	    /**
	     *
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
	    ///
	    std::mutex lockJobList;
	     /// The condition variable signals that a job has been inserted in the queue
        std::condition_variable notifyJob;
};

ARA_NAMESPACE_END

#endif
