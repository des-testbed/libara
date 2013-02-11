#include "TimeMock.h"

using namespace ARA;

TimeMock::TimeMock(){}

/**
 *
 */
TimeMock::TimeMock(struct timeval* timestamp):Time(timestamp){}

TimeMock::~TimeMock(){}

/**
 *
 */
void TimeMock::usleep(int seconds){
    // create a new timestamp
    struct timeval *timestamp = new timeval;
    // copy over the old values
    timestamp->tv_sec = this->getTimestamp()->tv_sec;
    timestamp->tv_usec = this->getTimestamp()->tv_usec;
    // add the sleep time
    timestamp->tv_usec += seconds;
    // updat ehte value
    this->setTimestamp(timestamp);
}

int TimeMock::toSeconds(){
    return this->getTimestamp()->tv_sec;
}

long int TimeMock::toMilliseconds(){
    return this->getTimestamp()->tv_usec;
}

void TimeMock::update(){
    /// this is a stub
}

void TimeMock::update(TimeMock t){
    this->update(t.getTimestamp());
}
