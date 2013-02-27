/*
 * $FU-Copyright$
 */

#include "Time.h"
#include <iostream>

using namespace ARA;

Time::Time(){
    this->timestamp.tv_sec = 0;
    this->timestamp.tv_usec = 0;
}

Time::Time(int seconds, long int microseconds){
   this->timestamp.tv_sec = seconds;
   this->timestamp.tv_usec = microseconds;
}

Time::Time(struct timeval timestamp):timestamp(timestamp){

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
//    this->timestamp = new timeval;
    /// set the members of the struct
    this->timestamp.tv_sec = other.getTimestamp().tv_sec;
    this->timestamp.tv_usec = other.getTimestamp().tv_usec;
}

Time::~Time(){ }

Time Time::subtract(const Time& right) const {
    struct timeval result = this->getTimeDifference(right);    
    return Time(result.tv_sec, result.tv_usec);
}

/**
 * The method determines the time difference between two timestamps. Here,
 * the member variable "timestamp" and a operand "time". The result is stored
 * in a timeval struct which is passed as an argument to the function
 *
 * The code is from the libc manual:
 *   http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
 *
 * @param right in The right operand of the computation.
 * @param result out The data structure which will hold the result of the 
 *   operation
 */
struct timeval Time::getTimeDifference(const Time& right) const {
    struct timeval result;

    struct timeval r;
    /// copy the content of the right operand
    r.tv_sec = right.getTimestamp().tv_sec;
    r.tv_usec = right.getTimestamp().tv_usec;

    if(this->timestamp.tv_usec < r.tv_usec){
        int nanoSeconds = (r.tv_usec - this->timestamp.tv_usec) / 1000000 + 1;
        r.tv_usec -= (1000000 * nanoSeconds);
        r.tv_sec  += nanoSeconds;
    }

    if(this->timestamp.tv_usec - r.tv_usec > 1000000){
        int nanoSeconds = (this->timestamp.tv_usec - r.tv_usec) / 1000000;
        r.tv_usec += (1000000 * nanoSeconds);
        r.tv_sec -= nanoSeconds;
    }

    result.tv_sec = this->timestamp.tv_sec - r.tv_sec; 
    result.tv_usec = this->timestamp.tv_usec - r.tv_usec; 

    return result;
}

int Time::getSeconds() const {
   return timestamp.tv_sec;
}

long int Time::getMilliSeconds() const {
   return timestamp.tv_sec * 1000;
}

struct timeval Time::getTimestamp() const {
  return timestamp;
}

void Time::setToCurrentTime(){
    gettimeofday(&this->timestamp, 0);
}

void Time::update(Time t){
    this->timestamp.tv_sec = t.getTimestamp().tv_sec;
    this->timestamp.tv_usec = t.getTimestamp().tv_usec;
}

void Time::setTimestamp(struct timeval timestamp){
    this->timestamp = timestamp;
}
