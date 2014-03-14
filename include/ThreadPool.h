/*
 * $FU-Copyright$
 */

#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include "ARAMacros.h"
#include "Queue.h"

#include <atomic>
#include <thread>
#include <vector>

ARA_NAMESPACE_BEGIN

class ThreadPool {
    public:
        ThreadPool(size_t numberOfThreads = std::thread::hardware_concurrency());
        virtual ~ThreadPool();

/*        template<typename FunctionType>
        void submit(FunctionType function){
            workQueue.push(std::function<void()>(function));
        }

*/
        void worker();

    private:
        ///
        std::atomic_bool finished;
        ///
        std::vector<std::thread> threads;
	///
	Queue<std::function<void()>> workQueue;
};

ARA_NAMESPACE_END

#endif
