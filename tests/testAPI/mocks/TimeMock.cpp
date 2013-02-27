#include "TimeMock.h"

#include <iostream>
#include <typeinfo>

using namespace ARA;

TimeMock::TimeMock(){
    this->timestamp = Time();
}

TimeMock::TimeMock(Time time){
    this->timestamp = time;
}

TimeMock::TimeMock(const TimeMock& other){
    this->timestamp = Time();
    /// this should hopefully call the copy constructor of class Time
    this->timestamp = other.getTimestamp();
}

Time TimeMock::operator-(const Time& right){
    Time result = Time();

    try{
        const TimeMock& r = dynamic_cast<const TimeMock&>(right);
        result = this->timestamp - r.getTimestamp();
        //std::cout << "TimeMock::operator-" << result.toSeconds() << " " << result.toMilliseconds() << std::endl;
    }catch(const std::bad_cast& exception){
        std::cerr << exception.what() << std::endl;
        std::cerr << "This object is not of type TimeMock but " << typeid(right).name() << std::endl;
        result = Time();
    }

    return result;
}

Time TimeMock::operator-=(const Time& right){
    try{
        const TimeMock& r = dynamic_cast<const TimeMock&>(right);
        this->timestamp = this->timestamp - r.getTimestamp();
    }catch(const std::bad_cast& exception){
        std::cerr << exception.what() << std::endl;
        std::cerr << "This object is not of type TimeMock but " << typeid(right).name() << std::endl;
    }
    // FIXME
    return this->timestamp;
}

/**
 * The usleep method simply adds the milliseconds passed as 
 * an argument to the function to the timestamp member variable
 * of class Time. Despite the original usleep function, the time
 * scale is milliseconds and not microseconds!
 *
 * @param in milliseconds The time in milliseconds which will be added to the
 *   timestamp
 */
void TimeMock::usleep(int milliseconds){
    struct timeval time;

    // copy over the old values
    time.tv_sec = this->timestamp.getTimestamp().tv_sec;
    time.tv_usec = this->timestamp.getTimestamp().tv_usec;
    // add the sleep time
    time.tv_sec += milliseconds/1000;
    // FIXME
    time.tv_usec += 0;

    this->timestamp.setTimestamp(time);
}

Time TimeMock::getTimestamp() const{
    return this->timestamp;
}

int TimeMock::getSeconds(){
    return this->timestamp.getSeconds();
}

long int TimeMock::getMilliSeconds(){
    return this->timestamp.getMilliSeconds();
}

void TimeMock::update(){
    /// this is a stub
}

void TimeMock::update(TimeMock t){
    this->timestamp.update(t.getTimestamp());
}

void TimeMock::initialize(){
    /// this is a stub
}

bool TimeMock::isInitialized(){
    return true;
}
