#include "ThreadPool.h"

ARA_NAMESPACE_BEGIN

ThreadPool::ThreadPool(size_t numberOfThreads){
    try {
        for (unsigned int i = 0; i < numberOfThreads; i++) {
            threads.push_back(std::thread(&ThreadPool::worker, this));
        }
    } catch (...) {
        finished = true;
        throw;
    }
}

ThreadPool::~ThreadPool(){
    finished = true;
}

void ThreadPool::worker(){
    while (!finished) {
        std::function<void()> task;

        if (workQueue.tryPop(task)) {
            task();
        } else {
            std::this_thread::yield();
        }
    }
}

ARA_NAMESPACE_END
