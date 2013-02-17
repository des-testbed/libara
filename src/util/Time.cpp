/******************************************************************************
 Copyright 2012, The DES-ARA-SIM Team, Freie Universität Berlin (FUB).
 All rights reserved.

 These sources were originally developed by Friedrich Große, Michael Frey
 at Freie Universität Berlin (http://www.fu-berlin.de/),
 Computer Systems and Telematics / Distributed, Embedded Systems (DES) group
 (http://cst.mi.fu-berlin.de/, http://www.des-testbed.net/)
 ------------------------------------------------------------------------------
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, either version 3 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program. If not, see http://www.gnu.org/licenses/ .
 ------------------------------------------------------------------------------
 For further information and questions please use the web site
 http://www.des-testbed.net/
 *******************************************************************************/
#include "Time.h"
#include <iostream>

using namespace ARA;

Time::Time(int seconds, long int microseconds){ 
   this->timestamp.tv_sec = seconds;
   this->timestamp.tv_usec = microseconds;
}


Time::Time(struct timeval timestamp):timestamp(timestamp){ }

/**
 * The standard constructor initializes the timestamp with the
 * value of zero. 
 */
Time::Time(){
    /// create a new timestamp
//    this->timestamp = new timeval;
    /// set the members of the struct to 0
//   memset(this->timestamp, 0, sizeof(timeval));
    this->timestamp.tv_sec = 0;
    this->timestamp.tv_usec = 0;
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

Time::~Time(){
//    delete this->timestamp;
}

Time Time::operator-(const Time& right){
    struct timeval result = this->getTimeDifference(right);    
    return Time(result.tv_sec, result.tv_usec);
}

Time Time::operator-=(const Time& right){
    struct timeval result = this->getTimeDifference(right);    
    this->timestamp.tv_sec = result.tv_sec;
    this->timestamp.tv_usec = result.tv_usec;
    return *(this);
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
struct timeval Time::getTimeDifference(const Time& right){
    struct timeval result;

    struct timeval r;
    /// copy the content of the right operand
    r.tv_sec = right.getTimestamp().tv_sec;
    r.tv_usec = right.getTimestamp().tv_usec;
/*
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
*/
    result.tv_sec = this->timestamp.tv_sec - r.tv_sec; 
    result.tv_usec = this->timestamp.tv_usec - r.tv_usec; 

    return result;
}

int Time::toSeconds(){
   return this->timestamp.tv_sec;
}

long int Time::toMilliseconds(){
   /**
    * The method will return 0 as long as tv_usec is below 1000
    * since the float (which is a result of the division) will be
    * cast to an long int. If a precision of milliseconds is to
    * low, the methods needs to be redone. 
    */
   return (this->timestamp.tv_usec/1000);
}

struct timeval Time::getTimestamp() const{
  return this->timestamp;
}

bool Time::isInitialized(){
    return (this->timestamp.tv_sec != 0);
}

void Time::update(){
    this->initialize();
}

void Time::update(Time t){
    this->timestamp.tv_sec = t.getTimestamp().tv_sec;
    this->timestamp.tv_usec = t.getTimestamp().tv_usec;
}

void Time::initialize(){
    gettimeofday(&this->timestamp, 0);
}

void Time::setTimestamp(struct timeval timestamp){
    this->timestamp = timestamp;
}
