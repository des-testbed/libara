#include "TimeMock.h"

using namespace ARA;

TimeMock::TimeMock(){}

/**
 *
 */
TimeMock::TimeMock(struct timeval* timestamp):Time(timestamp){}

/**
 *
 */
int TimeMock::usleep(int seconds){
  return 0;
}

int TimeMock::toSeconds(){
  return 0;
}
