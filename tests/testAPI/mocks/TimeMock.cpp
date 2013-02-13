#include "TimeMock.h"

using namespace ARA;

TimeMock::TimeMock():Time(){}

/**
 *
 */
TimeMock::TimeMock(struct timeval* timestamp):Time(timestamp){}

TimeMock::~TimeMock(){}

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
    struct timeval *timestamp = new timeval;
    // copy over the old values
    timestamp->tv_sec = this->getTimestamp()->tv_sec;
    timestamp->tv_usec = this->getTimestamp()->tv_usec;
    // add the sleep time
    timestamp->tv_sec += milliseconds/1000;
    timestamp->tv_usec += milliseconds * 1000;

    this->setTimestamp(timestamp);
}

int TimeMock::toSeconds(){
    return this->getTimestamp()->tv_sec;
}

long int TimeMock::toMilliseconds(){
    return (this->getTimestamp()->tv_usec/1000);
}

void TimeMock::update(){
    /// this is a stub
}

void TimeMock::update(TimeMock t){
    this->update(t.getTimestamp());
}
