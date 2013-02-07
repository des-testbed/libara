#include "Time.h"

using namespace ARA;

Time::Time(struct timeval *timestamp):timestamp(timestamp){ }

Time::Time(){
    this->timestamp = new timeval;
    /// get the current time
    memset(this->timestamp, 0, sizeof(timeval));
}

Time::~Time(){
    delete this->timestamp;
}

Time Time::operator-(const Time& right){
    ///
    struct timeval *result = new timeval;
    ///
    struct timeval *r = new timeval;
    memcpy(r, right.getTimestamp(), sizeof(timeval));
    
    if(this->timestamp->tv_usec < r->tv_usec){
        int nanoSeconds = (r->tv_usec - this->timestamp->tv_usec) / 1000000 + 1;
        r->tv_usec = r->tv_usec - (1000000 * nanoSeconds);
        r->tv_sec  = r->tv_sec + nanoSeconds;
    }

    if(this->timestamp->tv_usec - r->tv_usec > 1000000){
        int nanoSeconds = (this->timestamp->tv_usec - r->tv_usec) / 1000000;
        r->tv_usec = r->tv_usec + (1000000 * nanoSeconds);
        r->tv_sec  = r->tv_sec - nanoSeconds;
    }

    result->tv_sec = this->timestamp->tv_sec - r->tv_sec; 
    result->tv_usec = this->timestamp->tv_usec - r->tv_usec; 

    delete r;

    return Time(result);
}

Time Time::operator-=(const Time& right){
   // fixme
   return *(this);
}

int Time::toSeconds(){
   return this->timestamp->tv_sec;
}

long int Time::toMilliseconds(){
   return this->timestamp->tv_usec;
}

struct timeval* Time::getTimestamp() const{
  return this->timestamp;
}

