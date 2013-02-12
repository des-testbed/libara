#include "Time.h"

using namespace ARA;

Time::Time(struct timeval *timestamp):timestamp(timestamp){ }

Time::Time(){
    this->timestamp = new timeval;
    /// get the current time
    memset(this->timestamp, 0, sizeof(timeval));
}

/**
 * The copy constructor provides a copy of the object which is
 * passed as an argument to the copy constructor. Basically a
 * new instance of the member variable is created and the content
 * of the passed object is copied.
 *
 * @param in The object which should be copied
 */
Time::Time(const Time& other){
    /// allocate memory for the member variable
    this->timestamp = new timeval;
    /// set the members of the struct
    this->timestamp->tv_sec = other.getTimestamp()->tv_sec;
    this->timestamp->tv_usec = other.getTimestamp()->tv_usec;
}

Time::~Time(){
    delete this->timestamp;
}

Time Time::operator-(const Time& right){
    ///
    struct timeval *result = new timeval;
    ///
    struct timeval r;

    r.tv_sec = right.getTimestamp()->tv_sec;
    r.tv_usec = right.getTimestamp()->tv_usec;
    
    if(this->timestamp->tv_usec < r.tv_usec){
        int nanoSeconds = (r.tv_usec - this->timestamp->tv_usec) / 1000000 + 1;
        r.tv_usec = r.tv_usec - (1000000 * nanoSeconds);
        r.tv_sec  = r.tv_sec + nanoSeconds;
    }

    if(this->timestamp->tv_usec - r.tv_usec > 1000000){
        int nanoSeconds = (this->timestamp->tv_usec - r.tv_usec) / 1000000;
        r.tv_usec = r.tv_usec + (1000000 * nanoSeconds);
        r.tv_sec  = r.tv_sec - nanoSeconds;
    }

    result->tv_sec = this->timestamp->tv_sec - r.tv_sec; 
    result->tv_usec = this->timestamp->tv_usec - r.tv_usec; 

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

bool Time::isInitialized(){
    return (this->timestamp->tv_sec != 0);
}

void Time::update(){
    this->initialize();
}

void Time::update(Time t){
    this->timestamp->tv_sec = t.getTimestamp()->tv_sec;
    this->timestamp->tv_usec = t.getTimestamp()->tv_usec;
}

void Time::initialize(){
    gettimeofday(this->timestamp, 0);
}

void Time::setTimestamp(struct timeval* timestamp){
    delete this->timestamp;
    this->timestamp = timestamp;
}
