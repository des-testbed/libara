/*
 * $FU-Copyright$
 */

#ifndef TIMEOUT_EVENT_LISTENER_MOCK_H_
#define TIMEOUT_EVENT_LISTENER_MOCK_H_

#include "TimeoutEventListener.h"

namespace ARA {
    class TimeoutEventListenerMock : public TimeoutEventListener {
    public:
        void timerHasExpired(std::weak_ptr<Timer> responsibleTimer);
        bool hasBeenNotified();

    private:
        bool hasListenerBeenNotified = false;
    };
}

#endif
