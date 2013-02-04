#include "LinearEvaporationPolicy.h"

using namespace ARA;

LinearEvaporationPolicy::LinearEvaporationPolicy():q(0.1),threshold(0.2),factor(1){ 
    this->lastAccessTime = new timeval;
    /// initialize the last access time with '0'
    memset(this->lastAccessTime, 0, sizeof(timeval));
}

LinearEvaporationPolicy::~LinearEvaporationPolicy(){
    delete this->lastAccessTime;
} 

bool LinearEvaporationPolicy::checkForEvaporation(){
    if(this->lastAccessTime->tv_sec != 0){
        struct timeval *now = new timeval;
        /// get the current time
        gettimeofday(now, 0);

        /// determine the difference between the last access time and now
        if(now->tv_usec < this->lastAccessTime->tv_usec) {
            int nanoSeconds = (this->lastAccessTime->tv_usec - now->tv_usec) / 1000000 + 1;
            this->lastAccessTime->tv_usec = this->lastAccessTime->tv_usec - (1000000 * nanoSeconds);
            this->lastAccessTime->tv_sec  = this->lastAccessTime->tv_sec + nanoSeconds;
        }

        if(now->tv_usec - this->lastAccessTime->tv_usec > 1000000) {
            int nanoSeconds = (now->tv_usec - this->lastAccessTime->tv_usec) / 1000000;
            this->lastAccessTime->tv_usec = this->lastAccessTime->tv_usec + (1000000 * nanoSeconds);
            this->lastAccessTime->tv_sec  = this->lastAccessTime->tv_sec - nanoSeconds;
        }

        /// compare the timestamps (only the seconds
        if((now->tv_sec - this->lastAccessTime->tv_sec) > 5){
            /// delete the last access time
            delete this->lastAccessTime;
            /// update the timestamp
            this->lastAccessTime = now;
            /// set the factor

            /// return the result
            return true;
        }

        /// delete the last access time
        delete this->lastAccessTime;
        /// update the timestamp
        this->lastAccessTime = now;
    }else{
        /// initialize the last access time
        gettimeofday(this->lastAccessTime, 0);
    } 
    return false;
}

/**
 *
 * 
 */
float LinearEvaporationPolicy::evaporate(float phi){
   /// do the computation 
   phi = pow(((1 - this->q) * phi), this->factor); 

   /// check if the result is below a threshold
   if(phi < this->threshold){
     /// set pheromone to 0
     phi = 0;
   }
   return phi;
}
