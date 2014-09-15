/*
 * $FU-Copyright$
 */

#include "StandardClock.h"
#include "StandardTime.h"
#include "StandardTimer.h"

#include "Context.h"

ARA_NAMESPACE_BEGIN

Time* StandardClock::makeTime(){
    std::lock_guard<std::recursive_mutex> lock(mutex);
    return new StandardTime();
}

TimerPtr StandardClock::getNewTimer(char timerType, void* contextObject){
    std::lock_guard<std::recursive_mutex> lock(mutex);
    /// create a proxy for the standard timer	    
    std::shared_ptr<StandardTimerProxy> callback = std::make_shared<StandardTimerProxy>(timerType, contextObject);
    /// set the identifier of the timer
    callback->setTimerIdentifier(timerList.size());

    std::cerr << "[SC] timer identifier has been set to " << callback->getTimerIdentifier() << " for timer " << PacketType::getAsString(timerType) << std::endl;

    /// create a standard timer
    std::shared_ptr<StandardTimer> timer = std::make_shared<StandardTimer>(timerType);
    /// set the callback to the standard proxy
    timer->setCallback(callback);
    /// save the timer to the timer list
    timerList.push_back(timer);
    /// return the proxy timer
    return callback;
}

/*
std::string StandardClock::listActiveTimers() {
    std::ostringstream result;

    std::lock_guard<std::mutex> lock(mutex);
    for (int i = 0; i < timerList.size(); i++) {
        result << timerList[i] 
    }

    return result.str();
}
*/

void StandardClock::scheduleTimer(unsigned long identifier, unsigned long timeoutInMicroseconds){
    std::lock_guard<std::recursive_mutex> lock(mutex);

    try {
        std::shared_ptr<StandardTimer> timer = timerList[identifier]; 
        timer->run(timeoutInMicroseconds);
    } catch (const std::out_of_range error) {
        std::cerr << "out of bound exception in class StandardClock " << error.what() << std::endl;
    }	    
}

void StandardClock::scheduleTimer(std::function<void()> timer){
    std::lock_guard<std::recursive_mutex> lock(mutex);
    threadPool.schedule(timer);
}

void StandardClock::interruptTimer(unsigned long identifier){
    std::lock_guard<std::recursive_mutex> lock(mutex);

    try {
        std::shared_ptr<StandardTimer> timer = timerList[identifier]; 
        timer->interrupt();
    } catch (const std::out_of_range error) {
        std::cerr << "out of bound exception in class StandardClock " << error.what() << std::endl;
    }	    
}

ARA_NAMESPACE_END
