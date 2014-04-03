/*
 * $FU-Copyright$
 */

#include "TimeoutEventListenerMock.h"

using namespace ARA;

void TimeoutEventListenerMock::timerHasExpired(std::weak_ptr<Timer> responsibleTimer) {
    hasListenerBeenNotified = true;
}

bool TimeoutEventListenerMock::hasBeenNotified() {
    return hasListenerBeenNotified;
}
