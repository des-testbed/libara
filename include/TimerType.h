/*
 * $FU-Copyright$
 */

#ifndef TIMER_TYPE_H_
#define TIMER_TYPE_H_

#include "ARAMacros.h"

ARA_NAMESPACE_BEGIN

class TimerType {
    public:
        enum TimerTypeEnum {
            NEIGHBOR_ACTIVITY_TIMER,
            ROUTE_DISCOVERY_TIMER,
            PANTS_TIMER,
            DELIVERY_TIMER,
            ROUTE_DISCOVERY_DELAY_TIMER,
            ACK_TIMER,
            INVALID_TIMER
        };

        static std::string getAsString(char type) {
            switch (type) {
                case TimerType::NEIGHBOR_ACTIVITY_TIMER : return "Neighbor Activity Timer";
                case TimerType::ROUTE_DISCOVERY_TIMER : return "Route Discovery Timer";
                case TimerType::PANTS_TIMER : return "PANTS Timer";
                case TimerType::DELIVERY_TIMER : return "Delivery Timer";
                case TimerType::ROUTE_DISCOVERY_DELAY_TIMER : return "Route Discovery Delay Timer";
                case TimerType::ACK_TIMER : return "Acknowledgement Timer";
                case TimerType::INVALID_TIMER : return "Invalid Timer";
                default: return "UNKOWN TIMER";
            }
        }
};

ARA_NAMESPACE_END

#endif // TIMER_TYPE_H_
