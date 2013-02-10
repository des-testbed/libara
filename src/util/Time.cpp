#include "Time.h"

using namespace ARA;


void Time::getTimeDifference(struct timeval *a, struct timeval *b){
    if(a->tv_usec < b->tv_usec) {
        int nanoSeconds = (b->tv_usec - a->tv_usec) / 1000000 + 1;
        b->tv_usec = b->tv_usec - (1000000 * nanoSeconds);
        b->tv_sec  = b->tv_sec + nanoSeconds;
    }

    if(a->tv_usec - b->tv_usec > 1000000) {
        int nanoSeconds = (a->tv_usec - b->tv_usec) / 1000000;
        b->tv_usec = b->tv_usec + (1000000 * nanoSeconds);
        b->tv_sec  = b->tv_sec - nanoSeconds;
    }
}

int Time::getTimeDifferenceInSeconds(struct timeval *a, struct timeval *b){
    this->getTimeDifference(a, b);
    return (a->tv_sec - b->tv_sec);
}

int Time::getTimeDifferenceInMilliseconds(struct timeval *a, struct timeval *b){
    this->getTimeDifference(a, b);
    return (a->tv_usec - b->tv_usec);
}
