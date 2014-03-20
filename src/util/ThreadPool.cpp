#include "ThreadPool.h"

ARA_NAMESPACE_BEGIN

ThreadPool::ThreadPool(size_t numberOfThreads) : finished(false), workers(numberOfThreads) {
    try {
        for (std::unique_ptr<std::thread>& worker: workers) {
            worker.reset(new std::thread(&ThreadPool::worker, this));
        }
    } catch (...) {
        finished = true;
        throw;
    }
}

ThreadPool::~ThreadPool(){
    {
        std::unique_lock<std::mutex> lockList(lockJobList);
        finished = true;
        notifyJob.notify_all();
    }

    for (std::unique_ptr<std::thread>& worker : workers){
        worker->join();
    }
}

void ThreadPool::worker(){
    std::function<void()> job;

    for (;;) {
	{
            std::unique_lock<std::mutex> lockList(lockJobList);

	    for (;;) {

		if (finished) {
                    return;
	        }

		if (!jobs.empty()) {
                    job = jobs.front();
                    jobs.pop();
		    break;
		}

                notifyJob.wait(lockList);
	    }
	}

	job();
    }
}

void ThreadPool::schedule(std::function<void()> job){
    /// acquire the lock
    std::unique_lock<std::mutex> lockList(lockJobList);
    /// enqueue the job
    jobs.push(job);
    /// notify a waiting work
    notifyJob.notify_one();
}

ARA_NAMESPACE_END
