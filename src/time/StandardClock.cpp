/*
 * $FU-Copyright$
 */

#include "StandardClock.h"
#include "StandardTime.h"
#include "StandardTimer.h"

ARA_NAMESPACE_BEGIN

Time* StandardClock::makeTime(){
    return new StandardTime();
}

TimerPtr StandardClock::getNewTimer(TimerType timerType, void* contextObject){
    /// create a proxy for the standard timer	    
    std::shared_ptr<StandardTimerProxy> result = std::make_shared<StandardTimerProxy>(timerType, contextObject);
    /// set the identifier of the timer
    result->setTimerIdentifier(timerList.size());
    /// create a standard timer and append it to the list
    std::shared_ptr<StandardTimer> timer = std::make_shared<StandardTimer>(timerType);
    /// create a weak pointer to the proxy timer 
    std::weak_ptr<StandardTimerProxy> callback = result;
    /// set the callback to the standard proxy
    timer->setCallback(callback);
    /// save the timer to the timer list
    timerList.push_back(timer);
    /// return the proxy timer
    return result;
}

void StandardClock::scheduleTimer(unsigned long identifier, unsigned long timeoutInMicroseconds){
    try {
	std::shared_ptr<StandardTimer> timer = timerList.at(identifier); 
	timer->run(timeoutInMicroseconds);
    } catch (const std::out_of_range error) {
        std::cerr << "out of bound exception in class StandardClock " << error.what() << std::endl;
    }	    
}

void StandardClock::scheduleTimer(std::function<void()> timer){
    threadPool.schedule(timer);
}

void StandardClock::interruptTimer(unsigned long identifier){
    try {
	std::shared_ptr<StandardTimer> timer = timerList.at(identifier); 
	timer->interrupt();
    } catch (const std::out_of_range error) {
        std::cerr << "out of bound exception in class StandardClock " << error.what() << std::endl;
    }	    
}

ARA_NAMESPACE_END
