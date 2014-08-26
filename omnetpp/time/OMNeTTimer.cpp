/*
 * $FU-Copyright$
 */

#include "omnetpp/OMNeTTimer.h"

using namespace ARA;
using namespace ARA::omnetpp;

OMNeTTimer::OMNeTTimer(unsigned int id, OMNeTClock* clock, char type, void* contextObject) : Timer(type, contextObject) {
    this->id = id;
    this->clock = clock;
}

OMNeTTimer::~OMNeTTimer() {
    // notify the clock so all pending messages are canceled
    clock->timerHasBeenDeleted(id);
}

void OMNeTTimer::run(unsigned long timeoutInMicroSeconds) {
    clock->startTimer(id, timeoutInMicroSeconds);
}

void OMNeTTimer::interrupt() {
    clock->stopTimer(id);
}

void OMNeTTimer::notifyTimeExpired() {
    notifyAllListeners();
}

bool OMNeTTimer::equals(const Timer* otherTimer) const {
    const OMNeTTimer* otherOMNeTTimer = dynamic_cast<const OMNeTTimer*>(otherTimer);
    if (otherOMNeTTimer == nullptr) {
        return false;
    } 
    return (this->getHashValue() == otherOMNeTTimer->getHashValue());
}

bool OMNeTTimer::equals(const std::shared_ptr<Timer> otherTimer) const {
    return this->equals(otherTimer.get());
}

size_t OMNeTTimer::getHashValue() const {
    std::hash<void*> voidPtrHash;
    return voidPtrHash(contextObject);
}
