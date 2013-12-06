/*
 * $FU-Copyright$
 */

#ifndef _STANDARD_TIMER_THREAD_H_
#define _STANDARD_TIMER_THREAD_H_

#include <thread>

ARA_NAMESPACE_BEGIN

class StandardTimerThread {
    private:
	std::thread& timerThread;

    public:
        StandardTimerThread(std::thread& thread) : timerThread(thread) {} 
        ~StandardTimerThread(){
            if (timerThread.joinable()) {
                timerThread.join();
	    }
	}

	StandardTimerThread(StandardTimerThread const&)=delete;
	StandardTimerThread& operator=(StandardTimerThread const&)=delete;

};

#endif

ARA_NAMESPACE_END
