#include "TimeMock.h"

#include <iostream>
#include <typeinfo>

using namespace ARA;

TimeMock::TimeMock(){
    this->timestamp = new Time();
}

TimeMock::TimeMock(Time* time){
    this->timestamp = time;
}

Time TimeMock::operator-(const Time& right){
    Time result;

    try{
        const TimeMock& r = dynamic_cast<const TimeMock&>(right);
        result = (*(this->timestamp) - r.getTimestamp());
        std::cout << "TimeMock::operator-" << result.toSeconds() << " " << result.toMilliseconds() << std::endl;
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
        *(this->timestamp) = *(this->timestamp) - r.getTimestamp();
    }catch(const std::bad_cast& exception){
        std::cerr << exception.what() << std::endl;
        std::cerr << "This object is not of type TimeMock but " << typeid(right).name() << std::endl;
    }
    // FIXME
    return *(this->timestamp);
}

TimeMock::~TimeMock(){
    delete this->timestamp;
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
    // create a new timestamp
    struct timeval *time = new timeval;

    // copy over the old values
    time->tv_sec = this->timestamp->getTimestamp()->tv_sec;
    time->tv_usec = this->timestamp->getTimestamp()->tv_usec;

    // add the sleep time
    time->tv_sec += milliseconds/1000;
    time->tv_usec += milliseconds * 1000;

    this->timestamp->setTimestamp(time);
}

Time TimeMock::getTimestamp() const{
    return *(this->timestamp);
}

int TimeMock::toSeconds(){
    return this->timestamp->toSeconds();
}

long int TimeMock::toMilliseconds(){
    return this->timestamp->toMilliseconds();
}

void TimeMock::update(){
    /// this is a stub
}

void TimeMock::update(TimeMock t){
    this->timestamp->update(t.getTimestamp());
}

void TimeMock::initialize(){
    /// this is a stub
}

bool TimeMock::isInitialized(){
    return true;
}
